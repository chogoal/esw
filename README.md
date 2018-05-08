# Embedded Software

2018 1학기 건국대학교 **임베디드 소프트웨어** 수업의 소스코드를 정리한 것입니다.



## challenge1

`extern` 을 사용해보는 코드

* 첫 번째 모듈에서 정수형 변수 하나를 Export하고 두 번째 모듈에서 해당 변수를 출력하는 커널 모듈 코드

* 첫 번째 모듈의 IOCTL을 이용하여 위에서 선언한 정수형 변수의 값을 증가시키는 응용 코드

  <br>



## challenge2

설치시 3개의 LED가 1초마다 순차적으로 점등되는 모듈

<br>

## assignment1

커널함수를 통해 System V message Queue와 같은 방식으로 동작하는 IPC통신 



`ku_ipc.c` : 커널 모듈코드

`ku_ipc.h` : ku_ipc.c에 대한 header 파일

`ku_ipc_lib.c` : user process 영역에서 구동될 라이브러리 파일

`ku_ipc_mknod.sh` : 디바이스 등록 shell 파일

```
make
make copy
ssh pi@<Address of pi>
sudo su
sh ku_ipc_mknod.sh
insmod ku_ipc.ko
```

<br>

## assignment2

인체 감지 센서를 사용하여 공간에 사람이 있는 시간을 모니터링 하기 위한 디바이스 및 라이브러리

`ku_pir.c` : 커널 모듈코드

`ku_pir.h` : ku_pir.c에 대한 header 파일

`ku_pir_lib.c` : user process 영역에서 구동될 라이브러리 파일

`ku_pir_mknod.sh` : 디바이스 등록 shell 파일

```
make
make copy
ssh pi@<Address of pi>
sudo su
sh ku_pir_mknod.sh
insmod ku_pir.ko
```

<br>