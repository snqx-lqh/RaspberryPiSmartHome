#include "thread_ui.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "oled.h"
 
#define RUN_SUCCESS 0
#define RUN_FAIL -1

static ui_data_t ui_data;

char local_ip2[INET_ADDRSTRLEN] = {0};

void *thread_ui(void *parameter)
{
    uint8_t  showBuff[16] = {0}; 
    OLED_Init();
    OLED_Clear();
    OLED_Refresh();//更新显示
    if( get_local_ip_using_create_socket(local_ip2) == RUN_SUCCESS)
	{
		printf("%16sget_local_ip_using_socket() get local ip : %s \n","[thread_ui]:", local_ip2);
	}else{
		printf("%16sget_local_ip_using_socket() err \n","[thread_ui]:");
	}
    while (1)
    {
        sprintf((char*)showBuff,"TMP  :%3u",ui_data.tmp);
        OLED_ShowString(0,0,showBuff,16,1);
        sprintf((char*)showBuff,"RH   :%3u",ui_data.rh);
        OLED_ShowString(0,16,showBuff,16,1);
        sprintf((char*)showBuff,"LIGHT:%3u",ui_data.light_adc);
        OLED_ShowString(0,32,showBuff,16,1);
        OLED_ShowString(0,48,local_ip2,16,1);
        OLED_Refresh();//更新显示
        delay(100);
    }
}

void set_ui_data_dht11(uint8_t *RH_DATA,uint8_t *TMP_DATA)
{
    if(RH_DATA == NULL && TMP_DATA == NULL)
        return;
    ui_data.tmp = *RH_DATA;
    ui_data.rh  = *TMP_DATA;
}

void set_ui_data_light_adc(uint8_t *light_adc)
{
    if(light_adc == NULL)
        return;
    ui_data.light_adc = *light_adc;
}

ui_data_t get_ui_data()
{
    return ui_data;
}

int get_local_ip_using_create_socket(char *str_ip) 
{
	int status = RUN_FAIL;
	int af = AF_INET;
	int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in remote_addr;
	struct sockaddr_in local_addr;
	char *local_ip = NULL;
	socklen_t len = 0;
	
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_port = htons(53);
	remote_addr.sin_addr.s_addr = inet_addr("1.1.1.1");

	len =  sizeof(struct sockaddr_in);
	status = connect(sock_fd, (struct sockaddr*)&remote_addr, len);
	if(status != 0 ){
		printf("connect err \n");
	}
	
	len =  sizeof(struct sockaddr_in);
	getsockname(sock_fd, (struct sockaddr*)&local_addr, &len);
	
	local_ip = inet_ntoa(local_addr.sin_addr);
	if(local_ip)
	{
		strcpy(str_ip, local_ip);
		status = RUN_SUCCESS;
	}
	return status;
}
