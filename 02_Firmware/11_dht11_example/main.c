#include <wiringPi.h>

#include "dht11.h"
  
int main(void)
{
	uint8_t RH=0,TMP=0;
	int ret = 0;
    if (-1 == wiringPiSetupGpio()) {
        printf("Setup wiringPi failed!");
        return 1;
    }
	dht11_init();
    printf("Starting...\n");
    while (1) 
    {
        ret = get_dht11_data(&RH,&TMP);
		if(ret == 0)
		{
			printf("RH:%d TMP:%d\n",RH ,TMP); 
		}
        delay(1000);
    }
    return 0;
}