## 实验说明

我们这里会使用树莓派进行串口的收发。这个的作用就是有的时候没有网络的时候，就可以通过串口信息进行调试打印。但是暂时的智能家居项目可能不会用这个，只是扩展板画了，所以也做一个说明。

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

编写完成后使用之前的Makefile文件，make即可。

```bash
make
sudo ./main
```

想要停止这个程序，`Ctrl+c`即可。然后就可以使用上位机进行串口数据的发送，这个代码写的是一个回环。





