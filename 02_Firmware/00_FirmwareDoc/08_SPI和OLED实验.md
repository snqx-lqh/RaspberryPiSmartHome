## 实验说明

我们这里会使用SPI去驱动一个0.96的OLED，首先需要打开SPI

```
sudo raspi-config
Interfacing Options------>SPI------>Yes------->OK------->finsh
```

还是使用BCM编码。然后我们扩展板上的屏幕接到树莓派上，接mosi和sclk的脚，DC接24，RES接25，CS接CE0

我们可以使用OLED屏幕去显示一些树莓派信息，比如IP地址，比如传感器信息。

![image-20240817201702470](image/02_GPIO输出控制/image-20240817201702470.png)

## 代码编写

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

这是oled.c的代码

```c
/*********************用户修改区域****************************/
#include <wiringPi.h>
#include <wiringPiSPI.h>

#define LCD_DC  24     // 模式选择 1：写数据 0：写命令
#define LCD_DIN 10     // 串行数据线
#define LCD_CLK 11     // 串行时钟线
#define LCD_RST 25     // 复位信号  低电平有效
#define LCD_CE  8     // 芯片使能  低电平有效

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
    //uint8_t *tempData = &dat;
    wiringPiSPIDataRW(SPI_CHANNEL, &dat, 1);
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

void OLED_SPI_Init()
{
    pinMode(LCD_DC,OUTPUT);
    pinMode(LCD_RST,OUTPUT);

    int isOK = wiringPiSPISetup(SPI_CHANNEL, 500000);
    if (isOK == -1) {
        printf("SPI设置失败\n");
    }
    else {
        printf("SPI设置OK == %d\n",isOK);
    }
}

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

/*****************************************************************/
```

改了上面这部分，还要在oled初始化加上OLED_SPI_Init；

```c
//OLED的初始化
void OLED_Init(void)
{
    OLED_SPI_Init();
    ...
}
```

然后在main函数中使用一下这个函数。

```c
OLED_Init();
OLED_Clear();
OLED_ShowString(0,0,(char*)"HELLO 44444",16,1);
OLED_Refresh();//更新显示
```

编写完成后使用之前的Makefile文件，make即可。

```bash
make
sudo ./main
```

想要停止这个程序，`Ctrl+c`即可。

