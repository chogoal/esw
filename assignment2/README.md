# assignment2

인체감지센서(PIR)를 사용하여 사람이 있는 시간을 모니터링 하기 위한 디바이스 및 라이브러리

<br>

### 기능

인체감지센서가 작동시간을 Linked List를 사용해서 Kernel에 저장한다.

* IRQF_TRIGGER_RISING : 인체를 감지했을 경우
* IRGF_TRIGGER_FALLING : 감지중이던 인체를 놓쳤을 경우

<br>

### 요구사항

**정의되어야 할 속성**

`KUPIR_MAX_MSG` : 저장되는 감지 데이터 최대 개수  

`KUPIR_SENSOR 17` : 적외선 센서는 17번 핀을 사용한다

`DEV_NAME "ku_pir_dev"` : 디바이스 이름은 ku_pir_dev를 사용한다.

<br>

**구조체**

~~~c
struct ku_pir_data{
    long unsigned int timestamp;	/* 센서에 저장되는 timestamp */
    char rt_flag;				/* 센서의 RISING, FALLING 여부 */
}
~~~

<br>

### 파일설명

**ku_pir.c** : 커널 모듈코드

**ku_ipc.h** : ku_pir.c에 대한 header 파일

**ku_ipc_lib.c** : user process 영역에서 구동될 라이브러리 파일

**ku_ipc_mknod.sh** : 디바이스 등록 shell 파일

```
make
make copy
ssh pi@<Address of pi>
sudo su
sh ku_pir_mknod.sh
insmod ku_pir.ko
```

<br>

### ku_pir_lib.c

`ku_pir_open(void)` : LinkedList를 새로 생성하고 그 자료구조의 fd를 받아온다.

`ku_pir_close(int fd)` : fd에 해당하는 자료구조를 연결해제하고 삭제한다.

`ku_pir_read(int fd)` : fd에 해당하는 자료구조의 첫 번째 데이터를 읽어온다. 

`ku_pir_flush(int fd)` : fd에 해당하는 자료구조의 모든 데이터를 삭제한다.

`ku_pir_insertData(int fd, long unsigned int ts, char rt_flag)` : 테스트하기 위해 임의로 데이터를 삽입한다. 



