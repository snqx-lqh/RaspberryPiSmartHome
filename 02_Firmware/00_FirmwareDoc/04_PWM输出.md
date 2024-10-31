## PWM说明

树莓派的硬件引脚PWM有4个引脚。GPIO.1、GPIO.26、GPIO.23、GPIO.24，对应到WiringPi就是1、26、23、24。但是不同的代码库能支持的输出是不同的。

wiringPi库可以支持硬件PWM和软件PWM，但是硬件PWM有个问题就是，1和26会同时设置，23和24会同时设置，暂时不知道为什么，后面找到BUG会更新博文。

bcm2835库只支持硬件PWM，bcm的引脚编号18、12、13、19，我写代码测试的时候，发现18、12会同时输出，13、19会同时输出。暂时不知道为什么

Python库，我的版本，只支持GPIO.1的引脚的硬件输出

![image-20240817201702470](image/02_GPIO输出控制/image-20240817201702470.png)

## 代码

### wiringPi

wiringPi 可以使用硬件PWM，也可以软件PWM输出 

#### 硬件Pwm

树莓派内部pwm发生器的基频为19.2MHz，输出频率 **freq(Hz) = 19200000 / divisor / range**；

1、`void pwmSetMode (int mode) ;`

mode：PWM运行模式

pwm发生器可以运行在2种模式下，通过参数指定：

* PWM_MODE_BAL  ：树莓派默认的PWM模式

* PWM_MODE_MS  ：传统的pwm模式

我使用PWM_MODE_BAL怎么调都不对，就改成PWM_MODE_MS来使用了。

2、`void pwmSetRange (unsigned int range) ;`

range，范围的最大值 0~range，设置pwm发生器的数值范围，默认是1024

3、`void pwmSetClock (int divisor) `

设置时钟分频

4、`void pwmWrite(int pin, int value) ;`

设置占空比

下面是一个设置PWM输出的例子

```c
#include <wiringPi.h>
#include <stdio.h>

#define PWM 1

int main(int argc,char **argv)
{   
    if(wiringPiSetup() < 0) //当使用这个函数初始化树莓派引脚时，程序使用的是wiringPi 引脚编号表。
        return 1;
    
    pinMode(PWM,PWM_OUTPUT); //设置引脚为PWM输出模式
    pwmSetMode (PWM_MODE_MS);
    pwmSetRange(1024);              // pwm脉宽范围 0~1024
    pwmSetClock(75);                // 250Hz，19.2MHz / 75 / 1024 = 250Hz
    pwmWrite(PWM,512);              // 设置高电平占的计数值

    while (1)
    {

    }
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

然后可以用示波器啥的，或者自己连接的LED灯在这个引脚上，就可以查看到变化。、

想要停止这个程序，`Ctrl+c`即可。

#### 软件pwm

除了硬件PWM，该库还支持软件PWM的输出

1、`int softPwmCreate (int pin, int initialValue, int pwmRange)`

pin：用来作为软件PWM输出的引脚

initalValue：引脚输出的初始值

pwmRange：PWM值的范围上限 

返回：0表示成功。

2、`void softPwmWrite (int pin, int value)`

pin：通过softPwmCreate创建的引脚

value：PWM引脚输出的值



具体实现如下，频率设置为PWMfreq = 1 x 10^6 / (100 x range)

```c
#include <wiringPi.h>
#include <stdio.h>

#include <softPwm.h>

#define PWM 4

int main(int argc,char **argv)
{   
    if(wiringPiSetup() < 0) //当使用这个函数初始化树莓派引脚时，程序使用的是wiringPi 引脚编号表。
        return 1;
    
    pinMode(PWM,OUTPUT); //设置引脚为 输出模式
    //PWMfreq = 1 x 10^6 / (100 x range)  需要50hz 
    softPwmCreate(PWM , 0, 200);// 设置周期分为200份
    softPwmWrite(PWM , 50); //占空比就是50/200

    while (1)
    {

    }
    return 0;
}
```

### bcm2835库

bcm的引脚编号18、12、13、19，我写代码测试的时候，发现18、12会同时输出，13、19会同时输出。，暂时不知道为什么

1、`void bcm2835_pwm_set_clock(uint32_t divisor);`

设置时钟分频，分频方式有很多，可以转到定义处查看

2、`void bcm2835_pwm_set_mode(uint8_t channel, uint8_t markspace, uint8_t enabled);`

channel ：设置PWM是通道0还是1  

markspace：1是传统模式，0是BAL模式.

enabled：1为使能.

设置PWM模式

3、`void bcm2835_pwm_set_range(uint8_t channel, uint32_t range);`

channel ：设置PWM是通道0还是1  

range：设置最大范围.

4、`void bcm2835_pwm_set_data(uint8_t channel, uint32_t data);`

channel ：设置PWM是通道0还是1  

data：设置比例.



PWM输出频率 **freq(Hz) = 19200000 / divisor/ range**；

bcm的引脚编号和wiringPi不同，注意，下面是一个实际例子

```c
#include <bcm2835.h>

#define PWM 18
#define PWM_CHANNEL 0

int main(int argc,char **argv)
{
    if(!bcm2835_init()) //初始化BCM相关的
        return 1;

    // 配置引脚为PWM输出模式
    bcm2835_gpio_fsel(PWM, BCM2835_GPIO_FSEL_ALT5);
    
    // 时钟分频设置为16. 19.2Mhz/16 = 1.2Mhz
    bcm2835_pwm_set_clock(BCM2835_PWM_CLOCK_DIVIDER_16);
    // 设置模式
    bcm2835_pwm_set_mode(PWM_CHANNEL, 1, 1);
    // 1.2MHz/1024 = 1171.875Hz,  设置计数值
    bcm2835_pwm_set_range(PWM_CHANNEL, 1024);
    // 设置高电平时间
    bcm2835_pwm_set_data(PWM_CHANNEL, 256);

    while(1)
    {
         
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

然后可以用示波器啥的，就可以查看到变化。、

想要停止这个程序，`Ctrl+c`即可。

### RPi.GPIO

直接给代码了，python硬件只能设置BCM编号18的引脚

```python
#!/usr/bin/python
# -*- coding:utf-8 -*-
import RPi.GPIO as GPIO
import time

PWM = 18
 
GPIO.setmode(GPIO.BCM)     #设置编号方式
GPIO.setup(PWM, GPIO.OUT)  #设置 引脚为输出模式

p = GPIO.PWM(PWM, 50)  #将 引脚初始化为PWM实例 ，频率为50Hz
p.start(0)             #开始脉宽调制，参数范围为： (0.0 <= dc <= 100.0)

try:
    while 1:
        for dc in range(0, 101, 5):
            p.ChangeDutyCycle(dc)   #修改占空比 参数范围为： (0.0 <= dc <= 100.0)
            time.sleep(0.1)
        for dc in range(100, -1, -5):
            p.ChangeDutyCycle(dc)
            time.sleep(0.1)
except KeyboardInterrupt:
    pass

p.stop()          #停止输出PWM波
GPIO.cleanup()    #清除
```

