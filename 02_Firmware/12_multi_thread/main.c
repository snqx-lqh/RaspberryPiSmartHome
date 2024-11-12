#include <wiringPi.h>

#include "thread_key.h"
#include "thread_dht11.h"

#include <pthread.h>

pthread_t keyThread;       //按键线程
pthread_t dht11Thread;     //dht11线程

int main(void)
{
    if (-1 == wiringPiSetupGpio()) {
        printf("Setup wiringPi failed!");
        return 1;
    }

    pthread_create(&keyThread, NULL, thread_key, NULL);       // 按键线程创建 
    pthread_create(&dht11Thread, NULL, thread_dht11, NULL);   // dht11线程创建 
    
    pthread_join(keyThread, NULL);   // 调用pthread_join函数，使主线程阻塞并等待keyThread标识的线程执行完毕
    pthread_join(dht11Thread, NULL);

    return 0;
}