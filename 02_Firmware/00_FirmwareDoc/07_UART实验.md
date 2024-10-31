## 实验说明

我们这里会使用树莓派进行串口的收发。

```bash
sudo raspi-config
Interfacing Options------>Serial Port
```

会弹出两个弹框

第一个：Would you like a login shell to be accessible over serial?  选择否，把串口当作shell的

第二个：Would you like the serial port hardware to be enabled?   选择是，我们就可以控制硬件串口了

然后会让你重启，重启之后，查看串口

```bash
ls -l /dev/serial*
```

应该会发现，以下这种信息

```bash
lrwxrwxrwx 1 root root 5  8月18日 14:17 /dev/serial0 -> ttyS0
```

然后我们把USB转TTL接上树莓派，就可以来测试了，引脚是GPIO.8和GPIO.10

![image-20240817201702470](image/02_GPIO输出控制/image-20240817201702470.png)

## 代码编写

### wiringPi

主要说一下怎么初始化和发送数据，具体的实现，参考开源链接。

1、打开指定的串口，并设置波特率

```c
int   serialOpen      (const char *device, const int baud) ;
```

2、检查是否有数据

```c
int   serialDataAvail (const int fd) ;
```

3、获得一个数据

```c
int   serialGetchar   (const int fd) ;
```

4、发送一个数据

```c
void  serialPutchar   (const int fd, const unsigned char c) ;
```

5、发送一个字符串

```c
void  serialPrintf    (const int fd, const char *message, ...) ;
```

这是main.c的代码

```c
#include <stdio.h>
#include<wiringPi.h>
#include<wiringSerial.h>

int main(int argc,char **argv)
{
        int MySerial;

        if (wiringPiSetup()<0)
        {
            printf("Setup Failed!\n") ;
            return 1;
        }

        if ((MySerial=serialOpen("/dev/ttyS0", 115200))<0)
        {
            printf("Serial Failed!\n");
            return 1;
        }

        while (1)
        {
            if (serialDataAvail(MySerial)>0)
            {
                char ch=serialGetchar(MySerial);
                //serialPrintf(MySerial,"Hello World!\n"); 
                serialPutchar(MySerial,ch); 
            }
        }

        serialClose(MySerial);

        return 0;
}
```

然后编译这段代码

```bash
cc -Wall -o main main.c -lwiringPi
```

 -Wall 表示编译时显示所有警告，-lwiringPi 表示编译时动态加载 wiringPi 库

编译完成后调用生成的main文件

```bash
sudo ./main
```

然后就可以使用上位机进行串口数据的发送，这个代码写的是一个回环。

想要停止这个程序，`Ctrl+c`即可。

### bcm2835

该库不支持串口

### RPi.GPIO

直接给代码了，python应该比较好理解 

```python
#!/usr/bin/python
# -*- coding:utf-8 -*-
import serial
import time

ser = serial.Serial("/dev/ttyS0",115200)

if not ser.isOpen():
    print("open failed")
else:
    print("open success: ")
    print(ser)

try:
    while True:
        count = ser.inWaiting()
        if count > 0:
            recv = ser.read(count)
            print(recv)
            ser.write(recv)
        time.sleep(0.05) 
except KeyboardInterrupt:
    if ser != None:
        ser.close()
```

## linuxc语言访问文件

我们还可以，通过访问文件的方式访问串口并发送

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
 
#define BAUDRATE B115200 ///Baud rate : 115200
#define DEVICE "/dev/ttyS0"
#define SIZE 1024

int nFd = 0;
struct termios stNew;
struct termios stOld;
 
//Open Port & Set Port
int SerialInit()
{
    nFd = open(DEVICE, O_RDWR|O_NOCTTY|O_NDELAY);
    if(-1 == nFd)
    {
        perror("Open Serial Port Error!\n");
        return -1;
    }
    if( (fcntl(nFd, F_SETFL, 0)) < 0 )
    {
        perror("Fcntl F_SETFL Error!\n");
        return -1;
    }
    if(tcgetattr(nFd, &stOld) != 0)
    {
        perror("tcgetattr error!\n");
        return -1;
    }
 
    stNew = stOld;
    cfmakeraw(&stNew);//将终端设置为原始模式，该模式下所有的输入数据以字节为单位被处理
 
    //set speed
    cfsetispeed(&stNew, BAUDRATE);//115200
    cfsetospeed(&stNew, BAUDRATE);
 
    //set databits
    stNew.c_cflag |= (CLOCAL|CREAD);
    stNew.c_cflag &= ~CSIZE;
    stNew.c_cflag |= CS8;
 
    //set parity
    stNew.c_cflag &= ~PARENB;
    stNew.c_iflag &= ~INPCK;
 
    //set stopbits
    stNew.c_cflag &= ~CSTOPB;
    stNew.c_cc[VTIME]=0;    //指定所要读取字符的最小数量
    stNew.c_cc[VMIN]=1; //指定读取第一个字符的等待时间，时间的单位为n*100ms
                //如果设置VTIME=0，则无字符输入时read（）操作无限期的阻塞
    tcflush(nFd,TCIFLUSH);  //清空终端未完成的输入/输出请求及数据。
    if( tcsetattr(nFd,TCSANOW,&stNew) != 0 )
    {
        perror("tcsetattr Error!\n");
        return -1;
    }
 
    return nFd;
}
 
int main(int argc, char **argv)
{
    int nRet = 0;
    char buf[SIZE];
 
    if( SerialInit() == -1 )
    {
        perror("SerialInit Error!\n");
        return -1;
    }
 
    bzero(buf, SIZE);
    while(1)
    {
        nRet = read(nFd, buf, SIZE);
        if(-1 == nRet)
        {
            perror("Read Data Error!\n");
            break;
        }
        if(0 < nRet)
        {
            buf[nRet] = 0;
            printf("Recv Data: %s\n", buf);
            write(nFd, buf, SIZE);
        }
    }
 
    close(nFd);
    return 0;
}
```

然后编译这段代码

```bash
gcc -Wall main.c -o main  
```

 -Wall 表示编译时显示所有警告

编译完成后调用生成的main文件

```bash
sudo ./main
```

想要停止这个程序，`Ctrl+c`即可。

