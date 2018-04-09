#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");

#define LED1 4
#define LED2 19
#define LED3 16

static struct timer_list my_timer;

static void my_timer_func(unsigned long data){	
	if(data % 3 == 0){
		gpio_set_value(LED1,1);
		printk("LED1\n");
	}else if(data % 3 == 1){
		gpio_set_value(LED2,1);
		printk("LED2\n");
	}else if(data % 3 == 2){
		gpio_set_value(LED3,1);	
		printk("LED3\n");
	}
		my_timer.data = data + 1;
		my_timer.expires = jiffies + (1*HZ);
		gpio_set_value(LED1,0);		
		gpio_set_value(LED2,0);
		gpio_set_value(LED3,0);				
		add_timer(&my_timer);


}

static int __init simple_timer_init(void){
	printk("Timer init!\n");

	init_timer(&my_timer);
	
	gpio_request_one(LED1,GPIOF_OUT_INIT_LOW,"LED1");
	gpio_request_one(LED2,GPIOF_OUT_INIT_LOW,"LED2");
	gpio_request_one(LED3,GPIOF_OUT_INIT_LOW,"LED3");

	my_timer.function = my_timer_func;
	my_timer.data = 1;
	my_timer.expires = jiffies + (1*HZ);
	add_timer(&my_timer);
	return 0;
}

static void __exit simple_timer_exit(void){
	printk("Timer exit!\n");
	del_timer(&my_timer);
	gpio_free(LED1);
	gpio_free(LED2);
	gpio_free(LED3);	
}

module_init(simple_timer_init);
module_exit(simple_timer_exit);
