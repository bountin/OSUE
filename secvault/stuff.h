#include "ioctl.h"

int sv_open(struct inode *, struct file *);
ssize_t sv_read(struct file *, char __user *, size_t, loff_t *);
ssize_t sv_write(struct file *, const char __user *, size_t, loff_t *);
int sv_release(struct inode *, struct file *);
loff_t sv_seek(struct file *filp, loff_t off, int whence);

long sv_ctl_ioctl(struct file *, unsigned int, unsigned long);
int sv_ctl_open(struct inode *, struct file *);
int sv_ctl_release(struct inode *, struct file *);

struct file_operations sv_fops = {
    .owner = THIS_MODULE,
    .open = sv_open,
    .read = sv_read,
    .write = sv_write,
    .release = sv_release,
};

struct file_operations sv_ctl_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = sv_ctl_ioctl,
	.open  = sv_ctl_open,
	.release = sv_ctl_release,
};

struct sv_dev {
	char *data;  /* Pointer to encrypted memory - Set to NULL if uninitialized */
        unsigned long size;
	char key[KEY_LENGTH];
        struct semaphore sem;     /* mutual exclusion semaphore     */
        struct cdev cdev;     /* Char device structure      */
	int owner;
};
