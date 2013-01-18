#define KEY_LENGTH 10

struct sv_ioctl_message {
	int size;
	char key[KEY_LENGTH];
	int sv_id;
};
