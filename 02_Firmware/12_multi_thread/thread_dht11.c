#include "thread_dht11.h"

#include "dht11.h"

void *thread_dht11(void *parameter)
{
    uint8_t RH=0,TMP=0;
	int ret = 0;
    int thread_count = 0;
	dht11_init();
    printf("Starting...\n");
    while (1) 
    {
        ret = get_dht11_data(&RH,&TMP);
		if(ret == 0)
		{
            thread_count ++;
			printf("RH:%d TMP:%d thread_count:%d\n",RH ,TMP,thread_count); 
		}
        delay(1000);
    }
}