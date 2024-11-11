## 说明

树莓派的GPIO编程控制方式有使用wiringPi库、BCM2835库、RPi.GPIO库。

其中wiringPi库、BCM2835库是使用C++编程，RPi.GPIO是使用python编程方式。我们这个教程将使用WiringPi库进行开发，如果后续有需要，也可以介绍BCM2835和RPi.GPIO库。

这个章节将讲述如何使用GPIO控制LED灯的点亮和熄灭。我们可以用LED灯，模拟智能家居中的各个房间的灯的点亮，或者也可以模拟成一些开关。

我设计的智能家居扩展板一共有4个普通的用GPIO口控制的LED灯，分别是BCM编号，17、27、22和23。

## wiringPi函数库的安装

WiringPi是应用于树莓派平台的GPIO控制库函数，WiringPi中的函数类似于Arduino的Wiring系统。

安装相关的库

```bash
sudo apt-get update
sudo apt-get install build-essential
git clone https://github.com/WiringPi/WiringPi.git
# 如果不出意外，你的clone应该是成功不了，但是也有意外，反正就是看你的网能不能上github嘛，你可以自己下载后，然后使用MobaXterm将文件夹传上去，我在03_Tools文件夹提供了我记笔记时的wiringpi源代码，但是我的可能不是最新的。
# 如果上传压缩包，需执行
tar -vxzf WiringPi-3.10.tar.gz

cd WiringPi-3.10 
./build
```

然后就可以使用以下信息查看安装是否完成。

```bash
gpio -v
gpio readall
```

如果能够打印版本信息和引脚信息，即是完成了安装。

![image-20240817201702470](image/02_GPIO输出控制/image-20240817201702470.png)



## 控制GPIO口输出

首先，树莓派的GPIO口，不同的库给他的编号不同，有基本的功能名编的引脚，然后BCM库有一种编码，然后是wiringPi有一种编码。我们下面的代码将控制GPIO.1的输出高低电平的变换，他在BCM编码是17，在WiringPi是0，具体的编码对应可以看上面`gpio readall`打印的引脚信息。我的扩展版，使用了BCM编号的17、27、22、23作为LED的编号。

1、`int wiringPiSetup (void)`

返回:执行状态，-1表示失败

使用wiringPi时，你必须在执行任何操作前初始化树莓派，否则程序不能正常工作。当使用这个函数初始化树莓派引脚时，程序使用的是wiringPi 引脚编号表。引脚的编号为 0~16需要root权限

2、`int wiringPiSetupGpio (void)`

返回:执行状态，-1表示失败

当使用这个函数初始化树莓派引脚时，程序中使用的是BCM GPIO 引脚编号表。需要root权限

3、`void pinMode (int pin, int mode)`

pin：配置的引脚

mode:指定引脚的IO模式

可取的值：INPUT、OUTPUT、PWM_OUTPUT，GPIO_CLOCK

4、`void digitalWrite (int pin, int value)`

pin：控制的引脚

value：引脚输出的电平值。

可取的值：HIGH，LOW分别代表高低电平



下面就是写了一个使用wiringPi的控制0号端口高低电平变化的代码。使用BCM编码，wiringpi的0就是BCM的17，c文件名我命名为main.c

```c
#include <wiringPi.h>

#define LED 17

int main(void)
{
    if(wiringPiSetupGpio() < 0) 
        return 1;
    pinMode(LED,OUTPUT); //设置引脚为输出模式
    while (1)
    {
        digitalWrite(LED,1);
        delay(1000);//延时1000ms
        digitalWrite(LED,0);
        delay(1000);
    }
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

然后可以就可以查看到板子上LED灯的变化。

想要停止这个程序，`Ctrl+c`即可。

## 分文件编写

想要将led的驱动部分隔离出来，建立led.c和led.h文件。具体的写法参考开源代码。

分文件后，多文件的编译如下

```bash
cc -Wall -o main main.c bsp_led.c -lwiringPi
```

把bsp_led.c文件也加入进去。
