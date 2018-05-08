#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include "ku_pir.h"

int ku_pir_open();
int ku_pir_close(int fd);
void ku_pir_read(int fd, struct ku_pir_data *data);
void ku_pir_flush(int fd);
int ku_pir_insertData(int fd, long unsigned int ts, char rf_flag);

int ku_pir_open(){
	int dev = open("/dev/ku_pir_dev",O_RDWR);
	int return_val = (int)ioctl(dev,PIROPEN,0);
	close(dev);
	return return_val;
}

int ku_pir_close(int fd){
	int dev = open("/dev/ku_pir_dev",O_RDWR);
	int *value = &fd;
	int return_val = (int)ioctl(dev,PIRCLOSE,(unsigned long*)value);
	close(dev);
	return return_val;
}

void ku_pir_read(int fd,struct ku_pir_data *data){
	int dev = open("/dev/ku_pir_dev",O_RDWR);
	
	read_data *r_data = (read_data*)malloc(sizeof(read_data));
	r_data->data = (struct ku_pir_data*)malloc(sizeof(struct ku_pir_data));
	r_data->data = data;
	r_data->fd = fd;
	ioctl(dev,PIRREAD, (unsigned long*)r_data);
	close(dev);
}

void ku_pir_flush(int fd){
	int dev = open("/dev/ku_pir_dev",O_RDWR);
	int *value = &fd;
	
	ioctl(dev,PIRFLUSH,(unsigned long*)value);
	close(dev);
}

int ku_pir_insertData(int fd,long unsigned int ts, char rt_flag){
	int dev = open("/dev/ku_pir_dev",O_RDWR);
	int return_val;
	insert_data *i_data;
	i_data = (insert_data*)malloc(sizeof(insert_data));
	i_data->fd = fd;
	i_data->timestamp = ts;
	i_data->rt_flag = rt_flag;
	
	return_val = ioctl(dev,PIRINSERT,(unsigned long*)i_data);
	close(dev);
	return return_val;
}
