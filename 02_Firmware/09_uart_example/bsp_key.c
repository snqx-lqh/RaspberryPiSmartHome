#include "bsp_key.h"

#include <wiringPi.h>

void key_init()
{
    pinMode(KEY1_Pin,INPUT);         //设置引脚为输入模式
    pullUpDnControl(KEY1_Pin,PUD_UP);//设置引脚为上拉模式
    pinMode(KEY2_Pin,INPUT);         //设置引脚为输入模式
    pullUpDnControl(KEY2_Pin,PUD_UP);//设置引脚为上拉模式
    pinMode(KEY3_Pin,INPUT);         //设置引脚为输入模式
    pullUpDnControl(KEY3_Pin,PUD_UP);//设置引脚为上拉模式
}

/**
  * @brief   按键扫描，选择扫描的模式，然后放在一个5ms的任务中，参考正点原子按键扫描
  * @param   key_scan_mode：SINGLE_SCAN 单次扫描  CONTINUOUS_SCAN 连续扫描
  * @retval
 **/
uint8_t KeyScan(const key_scan_mode_t key_scan_mode)
{
    static int keyCount = 0;
    static int keyState = 0;
    if(key_scan_mode == CONTINUOUS_SCAN) keyState = 0;
    if (keyState == 0 && (KEY1 == 0||KEY2 == 0||KEY3 == 0  ))
    {    
        keyCount++;
        if(keyCount>2)
        {
            keyState = 1;
            keyCount=0;
            if(KEY1 == 0) return KEY1_VALUE;
            else if(KEY2 == 0) return KEY2_VALUE;
            else if(KEY3 == 0) return KEY3_VALUE;
        }
    }else if (KEY1 == 1 && KEY2 == 1 && KEY3 == 1 )
    {
        keyState = 0;
    }
	return 0;
}