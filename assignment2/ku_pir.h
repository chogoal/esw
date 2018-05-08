#define KUPIR_MAX_MSG 		5
#define KUPIR_SENSOR 		17
#define DEV_NAME 		"ku_pir_dev"

#define RISING 			'0'
#define FALLING 		'1'

#define IOCTL_START_NUM 	0x80
#define OPEN_NUM 		IOCTL_START_NUM+1
#define CLOSE_NUM 		IOCTL_START_NUM+2
#define READ_NUM 		IOCTL_START_NUM+3
#define FLUSH_NUM 		IOCTL_START_NUM+4
#define INSERT_NUM 		IOCTL_START_NUM+5


#define IOCTL_NUM 'h'
#define PIROPEN 		_IOWR(IOCTL_NUM, OPEN_NUM, unsigned long *)
#define PIRCLOSE 		_IOWR(IOCTL_NUM, CLOSE_NUM, unsigned long *)
#define PIRREAD		 	_IOWR(IOCTL_NUM, READ_NUM, unsigned long *)
#define PIRFLUSH		_IOWR(IOCTL_NUM, FLUSH_NUM, unsigned long *)
#define PIRINSERT		_IOWR(IOCTL_NUM, INSERT_NUM, unsigned long *)

struct ku_pir_data{
	long unsigned int timestamp;
	char rt_flag;
};

typedef struct insert_pir_data{
	int fd;
	long unsigned int timestamp;
	char rt_flag;
}insert_data;


typedef struct read_pir_data{
	int fd;
	struct ku_pir_data *data;
}read_data;

