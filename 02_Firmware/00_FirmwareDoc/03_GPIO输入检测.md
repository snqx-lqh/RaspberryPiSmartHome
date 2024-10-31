## 说明

首先，树莓派的GPIO口，不同的库给他的编号不同，有基本的功能名编的引脚，然后BCM库有一种编码，然后是wiringPi有一种编码。我们下面的代码将通过检测GPIO.0的输入高低电平变化来控制GPIO.1的输出高低电平的变换。

该笔记实现的功能，检测GPIO.0的输入，控制GPIO.1的输出

![image-20240817201702470](image/02_GPIO输出控制/image-20240817201702470.png)

## 代码

### wiringPi

1、`int digitalRead (int pin)`

pin：读取的引脚

返回：引脚上的电平，可以是LOW HIGH 之一



c文件名我命名为main.c，实现的功能就是按键端口检测为0，就设置LED端口电平为0，反之为1。

```c
#include <wiringPi.h>
#include <stdio.h>

#define LED 1
#define KEY 0

int main(void)
{
    int key_value = 0;

    if(wiringPiSetup() < 0) //当使用这个函数初始化树莓派引脚时，程序使用的是wiringPi 引脚编号表。
        return 1;
    
    pinMode(KEY,INPUT);  //设置引脚为输入模式
    pinMode(LED,OUTPUT); //设置引脚为输出模式
    
    while (1)
    {
        key_value = digitalRead(KEY); //读取引脚电平
        if(key_value == 0)
        {
            digitalWrite(LED,0);      //设置引脚电平为0
        }else
        { 
            digitalWrite(LED,1);      //设置引脚电平为1
        }  
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

然后可以用示波器啥的，或者自己连接的KEY和LED灯在这两个引脚上，就可以查看到变化。

想要停止这个程序，`Ctrl+c`即可。

### bcm2835库

1、`void bcm2835_gpio_set_pud(uint8_t pin, uint8_t pud);`

pin：配置的引脚

mode:指定引脚的上下拉模式，BCM2835_GPIO_PUD_OFF、BCM2835_GPIO_PUD_DOWN、BCM2835_GPIO_PUD_UP

2、`uint8_t bcm2835_gpio_lev(uint8_t pin);`

pin：配置的引脚

读取对应的引脚的值

bcm的引脚编号和wiringPi不同，注意，下面是一个实际例子

```c
#include <bcm2835.h>

#define LED 18
#define KEY 17

int main(int argc,char **argv)
{
    uint8_t keyValue = 0;
    if(!bcm2835_init()) //初始化BCM相关的
        return 1;

    //  设置KEY输入模式
    bcm2835_gpio_fsel(KEY, BCM2835_GPIO_FSEL_INPT);
    //  设置KEY上拉模式
    bcm2835_gpio_set_pud(KEY, BCM2835_GPIO_PUD_UP);
    //  设置LED输出模式
    bcm2835_gpio_fsel(LED,BCM2835_GPIO_FSEL_OUTP); 
    
    while(1)
    {
        keyValue = bcm2835_gpio_lev(KEY);
        if(keyValue == 0)
        {
            bcm2835_gpio_write(LED,LOW);      //设置引脚电平为0
        }else
        { 
            bcm2835_gpio_write(LED,HIGH);      //设置引脚电平为1
        }  
    }
    
    bcm2835_close();
    return 0;
}
```

然后编译这段代码

```bash
gcc -Wall main.c -o main -lbcm2835
```

 -Wall 表示编译时显示所有警告，-lbcm2835 表示编译时动态加载bcm2835 库

编译完成后调用生成的main文件

```bash
sudo ./main
```

然后可以用示波器啥的，或者自己连接的KEY和LED灯在这两个引脚上，就可以查看到变化。

想要停止这个程序，`Ctrl+c`即可。

### RPi.GPIO

直接给代码了 

```python
#!/usr/bin/python
# -*- coding:utf-8 -*-
import RPi.GPIO as GPIO
import time

LED = 18
KEY = 17

GPIO.setmode(GPIO.BCM)    #采用BCM编号方式
GPIO.setup(LED,GPIO.OUT)  #设置输出模式
GPIO.setup(KEY, GPIO.IN, pull_up_down=GPIO.PUD_UP)  #设置输入模式，输入上拉，下拉是GPIO.PUD_DOWN

keyValue = 0

try:
    while True:
        keyValue = GPIO.input(KEY)
        if keyValue:
            GPIO.output(LED,GPIO.HIGH)
        else:
            GPIO.output(LED,GPIO.LOW)
except:
    print("except")

GPIO.cleanup()
```

