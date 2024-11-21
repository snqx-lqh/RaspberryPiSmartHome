#include "thread_dht11.h"

#include "dht11.h"

#include "thread_ui.h"

void *thread_dht11(void *parameter)
{
    uint8_t RH=0,TMP=0;
	int ret = 0;
    int thread_count = 0;
	dht11_init();
    delay(3000);
    printf("%16sStarting...\n","[thread_dht11]:");
    while (1) 
    {
        ret = get_dht11_data(&RH,&TMP);
		if(ret == 0)
		{
            set_ui_data_dht11(&RH,&TMP);
            thread_count ++;
			printf("%16sRH:%d TMP:%d thread_count:%d\n","[thread_dht11]:",RH ,TMP,thread_count); 
		}
        //delay(1000);
        sleep(10);
    }
}