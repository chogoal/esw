#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/uaccess.h>

#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/rculist.h>

MODULE_LICENSE("GPL");
#include "ku_pir.h"

typedef struct pir_data{
	long unsigned int timestamp;
	char rt_flag;
	struct list_head list;	
}data;

typedef struct data_q{
	int fd;
	int capacity;	
	data *data;

	struct list_head list;
}data_q;

data_q qlist;
DECLARE_WAIT_QUEUE_HEAD(wait_q);
static int irq_num;

int *count;	/* for fd */
spinlock_t count_lock;	/* lock for count (fd) */

static int ku_pir_open(void);
static int ku_pir_close(int *user_fd);
static void make_queue(void);
static int ku_pir_insertData(insert_data *user_insert);
data_q* get_queue(int fd);
int is_full(int fd);
void ku_pir_flush(int *user_fd);
void ku_pir_read(read_data *user_read);
void del_first_data(data_q *temp_q);


static long ku_pir_ioctl(struct file *file, unsigned int cmd, unsigned long arg){	

	int return_val=-1;
	
	switch(cmd){
		case PIROPEN:
		{
			printk("PIR OPEN");
			ku_pir_open();
			return_val = *count;
			spin_lock(&count_lock);
			(*count)++;	
			spin_unlock(&count_lock);
			break;
		}
		case PIRCLOSE:
		{
			printk("PIR CLOSE");
			int *close_buf = (int*)arg;
			return_val = ku_pir_close(close_buf);
			break;
		}
		case PIRREAD:
		{
			printk("PIR READ");
			read_data *read_buf = (read_data*)arg;
			ku_pir_read(read_buf);
			break;
		}
		case PIRFLUSH:
		{
			printk("PIR FLUSH");
			int *flush_buf = (int*)arg;
			ku_pir_flush(flush_buf);
			return_val = 0;
			break;
		}
		case PIRINSERT:
		{
			printk("PIR INSERT");
			insert_data *insert_buf = (insert_data*)arg;
			return_val = ku_pir_insertData(insert_buf);
			break;
		}

		default:
		{
			return 0;
			break;
		}	
	}
	return (long) return_val;
 }

static int ku_pir_open(void){

	make_queue(); 
	return *count;
}

static int ku_pir_close(int *user_fd){

	int *fd = (int*)kmalloc(sizeof(int),GFP_KERNEL);
	copy_from_user(fd, user_fd, sizeof(int));

	data_q *temp_q = get_queue(*fd);
	
	data *temp_msg;
	struct list_head *pos;
	struct list_head *q;
	
	if(temp_q == NULL){	
		return -1;
	}else{
			list_for_each_safe(pos,q,&(temp_q->data->list)){
			temp_msg = list_entry(pos,data,list);
			list_del_rcu(pos);
			kfree(temp_msg);
		}

		list_for_each_safe(pos,q,&(qlist.list)){
			temp_q = list_entry(pos,data_q,list);
			if(temp_q->fd == *fd){
				list_del_rcu(&temp_q->list);
				kfree(temp_q);
			}

		}
	}
	wake_up(&wait_q);
	return 0;
}

void ku_pir_read(read_data *user_read){
	read_data *r_data = (read_data*)kmalloc(sizeof(read_data),GFP_KERNEL);
	r_data->data = (struct ku_pir_data*)kmalloc(sizeof(struct ku_pir_data),GFP_KERNEL);
	copy_from_user(r_data, user_read, sizeof(read_data));
	
	int fd = r_data->fd;
	data_q *temp_q = get_queue(fd);
	data *temp_msg;
	
	if(temp_q->capacity==KUPIR_MAX_MSG){
		/* non exclusive wait */		
		wait_event(wait_q,temp_q->capacity<KUPIR_MAX_MSG);
	}

	struct list_head *pos;
	struct list_head *q;

	
	list_for_each_safe(pos,q,&(temp_q->data->list)){
		rcu_read_lock();
		temp_msg = list_entry(pos,data,list);
		r_data->data->timestamp = temp_msg->timestamp;;
		r_data->data->rt_flag = temp_msg->rt_flag;
		copy_to_user(user_read,r_data,sizeof(read_data));
		rcu_read_unlock();
		list_del_rcu(pos);
		kfree(temp_msg);
		break;
	}
	temp_q->capacity += 1;
	wake_up(&wait_q);
}

void ku_pir_flush(int *user_fd){

	int *fd = (int*)kmalloc(sizeof(int),GFP_KERNEL);
	copy_from_user(fd, user_fd, sizeof(int));

	data_q *temp_q = get_queue(*fd);
	data *temp_msg;
	
	struct list_head *pos;
	struct list_head *q;

	list_for_each_safe(pos,q,&(temp_q->data->list)){
		temp_msg = list_entry(pos,data,list);
		list_del_rcu(pos);
		kfree(temp_msg);
	}
	temp_q->capacity = KUPIR_MAX_MSG;
}

static int ku_pir_insertData(insert_data *user_insert){
	
	insert_data *i_data = (insert_data*)kmalloc(sizeof(insert_data),GFP_KERNEL);
	copy_from_user(i_data,user_insert,sizeof(insert_data));

	data *temp_data = (data*)kmalloc(sizeof(data),GFP_KERNEL);
	temp_data->timestamp = i_data->timestamp;
	temp_data->rt_flag = i_data->rt_flag;
	
	int fd = i_data->fd;
	data_q *temp_q = get_queue(fd);

	if(temp_q == NULL){
		return -1;
	}else{
		if(is_full(fd) == 1){
			struct list_head *pos,*q;
			data *first_data;
			list_for_each_safe(pos,q,&temp_q->data->list){
				first_data = list_entry(pos,data,list);
				list_del_rcu(pos);
				kfree(first_data);
				break;
			}
			temp_q->capacity +=1;		
		}
	
	list_add_tail_rcu(&(temp_data->list),&(temp_q->data->list));

	temp_q->capacity -=1;

	wake_up(&wait_q);
	return 0;	
	}
	
	
}

void del_first_data(data_q *temp_q){
	
	struct list_head *pos,*q;
	data *first_data;
		list_for_each_safe(pos,q,&temp_q->data->list){
			first_data = list_entry(pos,data,list);
			list_del_rcu(pos);
			kfree(first_data);
			break;
		}
		temp_q->capacity +=1;
}

int is_full(int fd){
	data_q *temp_q = get_queue(fd);

	if(temp_q->capacity == 0){
		return 1;
	}else{
		return 0;
	}

}

static void make_queue(void){
	
	data_q *temp_q;
	temp_q = (data_q*)kmalloc(sizeof(data_q), GFP_KERNEL);
	temp_q->capacity = KUPIR_MAX_MSG;
	temp_q->fd = *count;
	
	temp_q->data = (data*)kmalloc(sizeof(data),GFP_KERNEL);	
	
	/* data list head init */	
	INIT_LIST_HEAD(&(temp_q->data->list));

	/* add queue in queuelist*/					
	list_add_rcu(&(temp_q->list),&(qlist.list));
}

data_q* get_queue(int fd){
	
	data_q *temp_q;

	struct list_head *pos ;

	rcu_read_lock();
	list_for_each(pos, &qlist.list) {
	    	temp_q = list_entry(pos, data_q, list);
		if(temp_q->fd == fd){
			return temp_q;
		}	
	}
	rcu_read_unlock();
	return NULL;
}

static irqreturn_t ku_pir_irq(int irq, void* dev_id){
	
	/*function - when recognize person*/
	data_q * temp_q = (data_q*)kmalloc(sizeof(data_q),GFP_KERNEL);
	temp_q->data = (data*)kmalloc(sizeof(data),GFP_ATOMIC);
	data *temp = (data*)kmalloc(sizeof(data),GFP_ATOMIC);
	
	struct list_head *pos ;
	
	temp->timestamp = jiffies;
	temp->rt_flag = '-1';

	int gpio_flag = gpio_get_value(KUPIR_SENSOR);
	if(gpio_flag == 0 ){
		printk("DETECT - RISING");
		temp->rt_flag = '0';

	}else if(gpio_flag == 1){
		printk("DETECT - FALLING");
		temp->rt_flag = '1';
	}	

	list_for_each(pos, &qlist.list) {
	    	temp_q = list_entry(pos, data_q, list);
		if(temp_q->capacity == 0){
			del_first_data(temp_q);
		}
		list_add_tail_rcu(&(temp->list),&(temp_q->data->list));
	}
	temp_q->capacity -= 1;
	return IRQ_HANDLED;	
}


static int open_func(struct inode *inode, struct file *file){
//	enable_irq(irq_num);
	return 0;
}

static int release_func(struct inode* inode, struct file *file){
//	disable_irq(irq_num);
	return 0;
}



struct file_operations ku_pir_fops = {
	.unlocked_ioctl = ku_pir_ioctl,
	.open = open_func,
	.release = release_func,
};

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init ku_pir_init(void){
	
	int ret;
	printk("Init Module\n");

	qlist.data = (data*)kmalloc(sizeof(data),GFP_KERNEL);	
	INIT_LIST_HEAD(&qlist.list);

	count = (int *)kmalloc(sizeof(int),GFP_KERNEL);
	spin_lock(&count_lock);
	*count = 0;
	spin_unlock(&count_lock);
	/* character device add */
	alloc_chrdev_region(&dev_num,0,1,DEV_NAME);
	cd_cdev = cdev_alloc();	
	cdev_init(cd_cdev,&ku_pir_fops);
	cdev_add(cd_cdev,dev_num,1);


	/* request GPIO and interrupt handler */
	gpio_request_one(KUPIR_SENSOR, GPIOF_IN, "sensor");
	irq_num = gpio_to_irq(KUPIR_SENSOR);
	ret = request_irq(irq_num, ku_pir_irq, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "sensor_irq", NULL);	

	return 0;

}

static void __exit ku_pir_exit(void){
	printk("Exit Module\n");
	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num,1);
	
	free_irq(irq_num, NULL);
	gpio_free(KUPIR_SENSOR);
}

module_init(ku_pir_init);
module_exit(ku_pir_exit);


