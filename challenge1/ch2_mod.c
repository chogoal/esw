#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/cdev.h>
MODULE_LICENSE("GPL");

#define DEV_NAME "ch2_mod_dev"
extern int num;


static int ch2_mod_read(struct file *file, char *buf, size_t len, loff_t *lof){
	printk("[ch2] current num : %d\n",num);
	return 0;
}

static int ch2_mod_open(struct inode *inode, struct file *file){
	printk("[ch2] open\n");
	return 0;
}

static int ch2_mod_release(struct inode *inode, struct file *file){
	printk("[ch2] release\n");
	return 0;
}

struct file_operations simple_char_fops = 
{
	.read = ch2_mod_read,
	.open = ch2_mod_open,
	.release = ch2_mod_release,
};

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init ch2_mod_init(void){
	printk("[ch2] init\n");
	printk("[ch2] current num : %d\n",num);
	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &simple_char_fops);
	cdev_add(cd_cdev, dev_num, 1);
	return 0;
}

static void __exit ch2_mod_exit(void){
	printk("Ecit Module\n");
	
	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);
}

module_init(ch2_mod_init);
module_exit(ch2_mod_exit);
