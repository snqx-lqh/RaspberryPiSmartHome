#include <wiringPi.h>

#include "thread_key.h"
#include "thread_dht11.h"
#include "thread_ui.h"
#include "thread_mqtt.h"
#include "thread_ad.h"

#include <pthread.h>

pthread_t keyThread;       //按键线程
pthread_t dht11Thread;     //dht11线程
pthread_t uiThread;        //ui线程
pthread_t mqttThread;      //mqtt线程
pthread_t adThread;        //ad线程

pthread_mutex_t pcfmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t dht11mutex = PTHREAD_MUTEX_INITIALIZER;

int main(void)
{
    if (-1 == wiringPiSetupGpio()) {
        printf("Setup wiringPi failed!");
        return 1;
    }

    pthread_mutex_init(&pcfmutex,NULL);
    pthread_mutex_init(&dht11mutex,NULL);

    pthread_create(&keyThread, NULL, thread_key, NULL);       // 按键线程创建 
    pthread_create(&dht11Thread, NULL, thread_dht11, NULL);   // dht11线程创建 
    pthread_create(&uiThread, NULL, thread_ui, NULL);         // ui线程创建 
    pthread_create(&mqttThread, NULL, thread_mqtt, NULL);         // mqtt线程创建 
    pthread_create(&adThread, NULL, thread_ad, NULL);         // mqtt线程创建 
    
    pthread_join(keyThread, NULL);   // 调用pthread_join函数，使主线程阻塞并等待keyThread标识的线程执行完毕
    pthread_join(dht11Thread, NULL);
    pthread_join(uiThread, NULL);
    pthread_join(mqttThread, NULL);
    pthread_join(adThread, NULL);

    pthread_mutex_destroy(&pcfmutex);
    pthread_mutex_destroy(&dht11mutex);

    return 0;
}