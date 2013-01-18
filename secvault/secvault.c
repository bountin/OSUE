#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <asm/uaccess.h>
#include <asm/string.h>

#include "svctl/svctl.h"
#include "stuff.h"

extern void *__memcpy(void*, void*, size_t);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Martin Prebio");

static struct sv_dev sv_devs[4];
static struct sv_dev sv_ctl_dev[1];

static int debug = 0;
module_param(debug, bool, S_IRUGO);

#define DEBUG(...) do { if (debug != 0) printk(KERN_NOTICE __VA_ARGS__); } while(0)

static void sv_setup_cdev(struct sv_dev *dev, int index)
{
	int err, devno = MKDEV(231, index+1); /* Index is the index of the array and not the minor number! */

	cdev_init(&dev->cdev, &sv_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &sv_fops;
	err = cdev_add(&dev->cdev, devno, 1);
	if (err)
		printk(KERN_ALERT "Error %d adding sv%d", err, index);

	dev->data = NULL;
	sema_init(&dev->sem, 1);
}

static void sv_setup_ctl_dev(struct sv_dev *dev)
{	/* Reuse the sv_dev struct ... */
	int err, devno = MKDEV(231, 0);

	cdev_init(&dev->cdev, &sv_ctl_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &sv_ctl_fops;
	err = cdev_add(&dev->cdev, devno, 1);
	if (err)
		printk(KERN_ALERT "Error %d adding sv_ctl", err);

	dev->data = NULL;
}

/******************************************************
 ********* Implementation of the sv operations ********
 ******************************************************/

int sv_open(struct inode *inode, struct file *filp)
{
	struct sv_dev *dev;
	dev = container_of(inode->i_cdev, struct sv_dev, cdev);

	if (dev->data == NULL) {
		/* Access denied since not initialized */
		return -EBUSY;
	}
	filp->private_data = dev; /* for other methods */
	DEBUG("SV: open");
	return 0;
}

ssize_t sv_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	ssize_t retval = 0;
	struct sv_dev *dev = filp->private_data;
	char *buffer = NULL;

	if (dev->owner != current_uid())
		return -EACCES;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;	
	if (*f_pos > dev->size)
		goto out;
	if (*f_pos + count > dev->size) /* Prevent reading over last content */
		count = dev->size - *f_pos;
	if (count > KEY_LENGTH)
		count = KEY_LENGTH;

	DEBUG("SV: read: %d %d", (int)count, (int)*f_pos);

	buffer = kmalloc(count, GFP_KERNEL);
	__memcpy(buffer, dev->data + *f_pos, count);
	for (int i = 0; i < count; i++) {
		buffer[i] ^= dev->key[i];
	}
	if (copy_to_user(buf, buffer, count)) {
		retval = -EFAULT;
		goto out;
	}

	*f_pos += count;
	retval = count;

	out:
	up(&dev->sem);
	kfree(buffer);
	return retval;
}

ssize_t sv_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	ssize_t retval = 0;
	struct sv_dev *dev = filp->private_data;
	char *buffer = NULL;

	if (dev->owner != current_uid())
		return -EACCES;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;
	if (*f_pos > dev->size)
		goto out;
	if (*f_pos + count > dev->size) /* Prevent buffer overflow */
		count = dev->size - *f_pos;
	if (count > KEY_LENGTH)
		count = KEY_LENGTH;

	DEBUG("SV: write: %d %d", (int)count, (int)*f_pos);
	buffer = kmalloc(count, GFP_KERNEL);
	if (copy_from_user(buffer, buf, count)) {
		retval = -EFAULT;
		goto out;
	}
	for (int i = 0; i < count; i++) {
		buffer[i] ^= dev->key[i];
	}
	__memcpy(dev->data + *f_pos, buffer, count);
	*f_pos += count;
	retval = count;

	out:
	kfree(buffer);
	up(&dev->sem);
	return retval;
}

loff_t sv_seek(struct file *filp, loff_t off, int whence)
{
	struct sv_dev *dev = filp->private_data;
	loff_t newpos;

	switch (whence) {
	case 0: /* SEEK_SET */
		newpos = off;
		break;
	case 1: /* SEEK_CUR */
		newpos = filp->f_pos + off;
		break;
	case 2: /* SEEK_END */
		newpos = dev->size + off;
		break;
	default: /* Can't happen */
		return -EINVAL;
	}

	if (newpos < 0)
		return -EINVAL;

	filp->f_pos = newpos;
	return newpos;
}

int sv_release(struct inode *inode, struct file *filp)
{
	return 0;
}

/**********************************************************
 ********* Implementation of the sv_ctl operations ********
 **********************************************************/

int sv_ctl_open(struct inode *inode, struct file *filp)
{
	return 0;
}

int sv_ctl_release(struct inode *inode, struct file *filp) 
{
	return 0;
}

long sv_ctl_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	if (cmd != SV_IOCREATE && cmd != SV_IOINIT && cmd != SV_IODELETE) {
		printk(KERN_ALERT "ioctl: Unknown command received\n");
		return -ENOTTY;
	}

	struct sv_ioctl_message message;
	copy_from_user(&message, (void *)arg, sizeof(struct sv_ioctl_message));

	DEBUG("SV: Got ioctl: %d size, %d id", message.size, message.sv_id);

	struct sv_dev* dev = &sv_devs[message.sv_id];

	switch (cmd) {
	case SV_IOCREATE:
		DEBUG("svctl: Create");
		if (dev->data != NULL)
			return -EADDRINUSE;
		if (message.size > 1024 * 1024)
			message.size = 1024*1024;

		dev->data = kmalloc(message.size, GFP_KERNEL);
		dev->size = message.size;
		memset(dev->data, 0, dev->size);
		__memcpy(dev->key, message.key, KEY_LENGTH);
		dev->owner = current_uid();
		break;
	case SV_IOINIT:
		DEBUG("svctl: Init");
		if (dev->data == NULL)
			break;
		memset(dev->data, 0, dev->size);
		break;
	case SV_IODELETE:
		DEBUG("svctl: Delete");
		/* Even if dev is not init'ed this procedure is idempotent */
		kfree(dev->data);
		dev->data = NULL;
		break;
	default: /* Can't happen - Case is caught above */
		return -ENOTTY;
	}

	return 0;
}


/********************* Module init stuff ********************/

static int __init mod_init(void)
{
	DEBUG("SV Module loading");

	int err;

	err = register_chrdev_region(MKDEV(231,0), 5, "secvault");
	if (err < 0) {
		printk(KERN_ALERT "register_chrdev_region failed");
		return err; 
	}

	sv_setup_ctl_dev(sv_ctl_dev);

	for (int i = 0; i < 4; i++) {
		sv_setup_cdev(sv_devs+i, i);
	}

	DEBUG("SV Module loaded");
	return 0;
}

static void __exit mod_exit(void)
{
	DEBUG("SV Module unloading");

	/* Remove existing devices */
	struct sv_dev *dev;
	for (int i = 0; i < 4; i++) {
		dev = &sv_devs[i];
		kfree(dev->data);
		cdev_del(&dev->cdev);
	}
	dev = sv_ctl_dev;
	cdev_del(&dev->cdev);

	unregister_chrdev_region(MKDEV(231,0), 5);
	DEBUG("SV Module unloaed");
}

module_init(mod_init);
module_exit(mod_exit);
