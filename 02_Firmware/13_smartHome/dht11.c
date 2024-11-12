#include "dht11.h"

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
 
#include <pthread.h>
#include <time.h>

#define HIGH_TIME 32
#define DHT11_PIN 4
#define TIMEOUT_COUNT 2000
#define MAX_WAIT_LOOP 1000  // 单次等待循环的最大次数限制

//注意执行get_dht11_data()的间隔不要太短，这里使用3s
static uint32_t databuf;

extern pthread_mutex_t dht11mutex;
  
void dht11_init()
{
    pinMode(DHT11_PIN, OUTPUT); // set mode to output
    digitalWrite(DHT11_PIN, 1); // output a high level 
}

uint8_t readSensorData(void)
{
    uint8_t crc; 
    uint8_t i;
  
    pinMode(DHT11_PIN, OUTPUT); // set mode to output
    digitalWrite(DHT11_PIN, 0); // output a high level 
    delay(25);
    digitalWrite(DHT11_PIN, 1); // output a low level 
    pinMode(DHT11_PIN, INPUT); // set mode to input
    pullUpDnControl(DHT11_PIN, PUD_UP);
 
    delayMicroseconds(27);
    if (digitalRead(DHT11_PIN) == 0) //SENSOR ANS
    {
        while (!digitalRead(DHT11_PIN))
            ; //wait to high
 
        for (i = 0; i < 32; i++)
        {
            while (digitalRead(DHT11_PIN))
                ; //data clock start
            while (!digitalRead(DHT11_PIN))
                ; //data start
            delayMicroseconds(HIGH_TIME);
            databuf *= 2;
            if (digitalRead(DHT11_PIN) == 1) //1
            {
                databuf++;
            }
        }
 
        for (i = 0; i < 8; i++)
        {
            while (digitalRead(DHT11_PIN))
                ; //data clock start
            while (!digitalRead(DHT11_PIN))
                ; //data start
            delayMicroseconds(HIGH_TIME);
            crc *= 2;  
            if (digitalRead(DHT11_PIN) == 1) //1
            {
                crc++;
            }
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

int get_dht11_data(uint8_t *RH_DATA,uint8_t *TMP_DATA)
{
    int ret = 0;
    int read_data = 0;
    pthread_mutex_lock(&dht11mutex);
    read_data = readSensorData();
    if (read_data)
    {
        // printf("Sensor data read ok!\n");
        // printf("RH:%d.%d\n", (databuf >> 24) & 0xff, (databuf >> 16) & 0xff); 
        // printf("TMP:%d.%d\n", (databuf >> 8) & 0xff, databuf & 0xff);
        
        *RH_DATA  = (uint8_t)((databuf >> 24) & 0xff);
        *TMP_DATA = (uint8_t)((databuf >> 8) & 0xff);
        databuf = 0;
        ret = 0;
    }
    else
    {
        databuf = 0;
        ret = -1;
    }
    pinMode(DHT11_PIN, OUTPUT); // set mode to output
    digitalWrite(DHT11_PIN, 1); 
    pthread_mutex_unlock(&dht11mutex);
    return ret;
}

