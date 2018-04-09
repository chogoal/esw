#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>

#define DEV_NAME "simple_ioctl_dev"

#define DEV_NAME "ch1_mod_dev"

#define CH1_START_NUM 0x80
#define CH1_NUM1 CH1_START_NUM+1
#define CH1_NUM2 CH1_START_NUM+2

#define SIMPLE_IOCTL_NUM 'z'
#define SIMPLE_IOCTL1 _IOWR(SIMPLE_IOCTL_NUM, CH1_NUM1, unsigned long *)
#define SIMPLE_IOCTL2 _IOWR(SIMPLE_IOCTL_NUM, CH1_NUM2, unsigned long *)

void main(void){
	int dev;
	unsigned long value = 0;

	dev = open ("/dev/ch1_mod_dev", O_RDWR);

	ioctl(dev,SIMPLE_IOCTL1, &value);
	ioctl(dev,SIMPLE_IOCTL2, &value);

	close(dev);
}
