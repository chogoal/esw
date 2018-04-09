#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/cdev.h>
MODULE_LICENSE("GPL");

#define DEV_NAME "ch1_mod_dev"

#define CH1_START_NUM 0x80
#define CH1_NUM1 CH1_START_NUM+1
#define CH1_NUM2 CH1_START_NUM+2

#define SIMPLE_IOCTL_NUM 'z'
#define SIMPLE_IOCTL1 _IOWR(SIMPLE_IOCTL_NUM, CH1_NUM1, unsigned long *)
#define SIMPLE_IOCTL2 _IOWR(SIMPLE_IOCTL_NUM, CH1_NUM2, unsigned long *)

int num;
EXPORT_SYMBOL(num);


static long ch1_mod(struct file *file, unsigned int cmd, unsigned long arg){
	switch(cmd){
	case SIMPLE_IOCTL1:
		printk("[ch1] current num : %d\n",num);

		break;
	case SIMPLE_IOCTL2:
		printk("[ch1] current num : %d\n",num);
		num++;
		printk("[ch1] current num : %d\n",num);
		break;

	default:
		return -1;
	}
	
	return 0;
}

static int ch1_mod_open(struct inode *inode, struct file *file){
	printk("[ch1] open\n");
	return 0;
}

static int ch1_mod_release(struct inode *inode, struct file *file){
	printk("[ch1] release\n");
	return 0;
}

struct file_operations simple_char_fops = 
{
	.unlocked_ioctl = ch1_mod,
	.open = ch1_mod_open,
	.release = ch1_mod_release,
};

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init ch1_mod_init(void){
	printk("[ch1] Init Module\n");
	num = 0;
	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &simple_char_fops);
	cdev_add(cd_cdev, dev_num, 1);
	return 0;
}

static void __exit ch1_mod_exit(void){
	printk("[ch1] Exit Module\n");
	
	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num,1);
}

module_init(ch1_mod_init);
module_exit(ch1_mod_exit);

