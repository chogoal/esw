# assignment1

커널함수를 통해 System V message Queue와 같은 방식으로 동작하는 IPC통신 구현

<br>

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