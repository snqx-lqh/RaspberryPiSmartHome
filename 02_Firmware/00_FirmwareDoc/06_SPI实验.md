## 实验说明

我们这里会使用SPI去驱动一个0.96的OLED，首先需要打开SPI

```
sudo raspi-config
Interfacing Options------>SPI------>Yes------->OK------->finsh
```

然后将屏幕接到树莓派上，接mosi和sclk的脚，DC接28，RST接29，CE接CE0

这个实验有一个坑我还没找到，就是每次我使用了BCM库来控制屏幕后，就不能使用Python和WringPi来控制了，这时候把SPI关一次再开一次才可以使用，但是如果一个时间内只掉用他们中的单独一个，是不会有问题的。

![image-20240817201702470](image/02_GPIO输出控制/image-20240817201702470.png)

## 代码编写

### wiringPi

主要说一下怎么初始化和发送数据，具体的实现，参考开源链接。

1、选择SPI的通道和速度。

`channel`只有0和1这两个值，表示初始化哪个`SPI`通道；

`speed`的范围是500000 到32000000，这个是SPI通讯的速度范围，单位是Hz；

```c
int wiringPiSPISetup     (int channel, int speed) ;
```

2、开始数据传输

`channel`只有0和1这两个值，表示初始化哪个`SPI`通道；
`data`要传输的字节，十六进制；
`len`传输的字节长度。

```c
int wiringPiSPIDataRW    (int channel, unsigned char *data, int len) ;
```

下面是一个初始化SPI的代码，然后使用的OLED.c就是中景元开源的代码，将SPI部分做了修改。

这是main.c的代码

```c
#include <wiringPi.h>
#include <stdio.h>
#include <wiringPiI2C.h>
#include "oled.h"
#include <wiringPiSPI.h>

#define LCD_DC  28     // 模式选择 1：写数据 0：写命令
#define LCD_DIN 12     // 串行数据线
#define LCD_CLK 14     // 串行时钟线
#define LCD_RST 29     // 复位信号  低电平有效
#define LCD_CE  10     // 芯片使能  低电平有效

#define SPI_CHANNEL 0

void oled_write_one_byte(uint8_t dat,uint8_t mode)
{ 
    digitalWrite(LCD_CE,LOW);
    if (mode == OLED_DATA) { // 写数据
        digitalWrite(LCD_DC,HIGH);
    }
    else { // 写命令
        digitalWrite(LCD_DC,LOW);
    }
    uint8_t *tempData = &dat;
    wiringPiSPIDataRW(SPI_CHANNEL, tempData, 1);
    digitalWrite (LCD_CE, HIGH);
}

void set_rst_level(uint8_t level)
{
    if(level == 1)
    {
        digitalWrite(LCD_RST,HIGH);
    }else
    {
        digitalWrite(LCD_RST,LOW);
    }
}

void delay_ms(uint8_t ms)
{
    delay(ms);
}

int main(void)
{ 
    if(wiringPiSetup() < 0) //当使用这个函数初始化树莓派引脚时，程序使用的是wiringPi 引脚编号表。
        return 1;

    pinMode(LCD_DC,OUTPUT);
    pinMode(LCD_RST,OUTPUT);
    pinMode(LCD_CE,OUTPUT);

    int isOK = wiringPiSPISetup(SPI_CHANNEL, 1000000);
    if (isOK == -1) {
        printf("SPI设置失败\n");
    }
    else {
        printf("SPI设置OK == %d\n",isOK);
    }
 
    OLED_Init();
    OLED_Clear();
    OLED_ShowString(0,0,(char*)"HELLO2",16,1);
    OLED_Refresh();//更新显示
 
    return 0;
}
```

这是oled.c主要修改的部分

```c
extern void oled_write_one_byte(uint8_t dat,uint8_t mode);
extern void set_rst_level(uint8_t level);
extern void delay_ms(uint8_t ms);

void OLED_WR_Byte(uint8_t dat,uint8_t mode)
{
    oled_write_one_byte(dat,mode);
}

void OLED_RST_SET(uint8_t level)
{
    set_rst_level(level);
}

void LCD_DELAY_MS(uint8_t ms)
{
    delay_ms(ms);
}

//OLED的初始化
void OLED_Init(void)
{
    OLED_RST_SET(0); // 液晶复位
    LCD_DELAY_MS(20);
    OLED_RST_SET(1);
    LCD_DELAY_MS(20);
    ...
}
```

然后编译这段代码

```bash
cc -Wall -o main main.c oled.c -lwiringPi
```

 -Wall 表示编译时显示所有警告，-lwiringPi 表示编译时动态加载 wiringPi 库

编译完成后调用生成的main文件

```bash
sudo ./main
```

想要停止这个程序，`Ctrl+c`即可。

### bcm2835

主要说一下怎么初始化和发送数据，具体的实现，参考开源链接。

1、开始SPI操作

```c
int bcm2835_i2c_begin(void);
```

2、设置高位在前还是低位在前

```c
void bcm2835_spi_setBitOrder(uint8_t order);
```

3、设置工作模式

```c
void bcm2835_spi_setDataMode(uint8_t mode);
```

4、设置分频系数

```c
void bcm2835_spi_setClockDivider(uint16_t divider);
```

5、选择控制哪个CS

```c
void bcm2835_spi_chipSelect(uint8_t cs);
```

6、选择激活时的CS电平，就是工作时候的CS电平

```c
void bcm2835_spi_setChipSelectPolarity(uint8_t cs, uint8_t active);
```

7、结束SPI的操作

```c
void bcm2835_spi_end(void);
```

注意，下面是一个实际例子，oled部分的处理和wiringPi一样这里就不写了。**注意两种库的引脚编号不一样**

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <bcm2835.h>
#include "oled.h"

#define LCD_DC  20     // 模式选择 1：写数据 0：写命令
#define LCD_DIN 10     // 串行数据线
#define LCD_CLK 11     // 串行时钟线
#define LCD_RST 21     // 复位信号  低电平有效
#define LCD_CE  8     // 芯片使能  低电平有效

void oled_write_one_byte(uint8_t dat,uint8_t mode)
{ 
    if (mode == OLED_DATA) { // 写数据
        bcm2835_gpio_write(LCD_DC,HIGH);
    }
    else { // 写命令
        bcm2835_gpio_write(LCD_DC,LOW);
    }
    bcm2835_spi_transfer(dat);
}

void set_rst_level(uint8_t level)
{
    if(level == 1)
    {
        bcm2835_gpio_write(LCD_RST,HIGH);
    }else
    {
        bcm2835_gpio_write(LCD_RST,LOW);
    }
}

void delay_ms(uint8_t ms)
{
    bcm2835_delay(ms);
}

int main(int argc, char **argv)
{
	if (!bcm2835_init())//所有外围io引脚初始化，之前已经分析过了
    {
        printf("bcm2835_init failed. Are you running as root??\n");
        return 1;
    }

    bcm2835_gpio_fsel(LCD_DC, BCM2835_GPIO_FSEL_OUTP); //设置为输出模式
    bcm2835_gpio_fsel(LCD_RST,BCM2835_GPIO_FSEL_OUTP); //设置为输出模式

    if (!bcm2835_spi_begin())    {
        printf("bcm2835_spi_begin failed. Are you running as root??\n");
        return 1;
    }

    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);  
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0); 
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_32);  
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW); 

    OLED_Init();
    OLED_Clear();
    OLED_ShowString(0,0,(char*)"HELLO3 world",16,1);
    OLED_Refresh();//更新显示
    
     
    bcm2835_spi_end();  
    bcm2835_close();
 
    return 0;
}
```

然后编译这段代码

```bash
gcc -Wall main.c oled.c -o main -lbcm2835
```

 -Wall 表示编译时显示所有警告，-lbcm2835 表示编译时动态加载bcm2835 库

编译完成后调用生成的main文件

```bash
sudo ./main
```

想要停止这个程序，`Ctrl+c`即可。

### RPi.GPIO

python控制iic的话，是有专门的别人写好的库。有两个库可以选择luma和Adafruit，但是我们下面的使用只使用luma。

```bash
pip install luma.oled
```

我们使用python安装的时候，应该会报错error: externally-managed-environment，网上有几种方法解决，可以参考[这篇博文](https://blog.csdn.net/2202_75762088/article/details/134625775)，我使用的是他说的创建venv的方式。

```bash
python -m venv ~/myenv        #创建虚拟环境，myenv就是环境名
source ~/myenv/bin/activate   #使能我们创建的虚拟环境
pip install luma.oled         #再次Pip安装
```

其他env操作

```
deactivate #退出环境
```

直接给代码了，python应该比较好理解 

```python
from luma.core.interface.serial import i2c, spi
from luma.core.render import canvas
from luma.oled.device import ssd1306

# 创建 SPI 设备，使用 SPI-0
serial = spi(device=0, port=0, gpio_RST=21, gpio_DC=20)

# 创建屏幕的驱动实例
device = ssd1306(serial)

# 开始往屏幕上绘图。draw 是 Pillow 的实例，它里面还有非常多的绘图 API。
with canvas(device) as draw:
  draw.rectangle(device.bounding_box, outline="white", fill="black")
  draw.text((30, 40), "Hello World!", fill="white")

# 这行是为了阻止程序退出，因为退出的时候会调用析构函数，清空屏幕。防止一闪而过，什么也看不到。
while (True):
  pass

```

更多资料可查看https://github.com/rm-hull/luma.examples