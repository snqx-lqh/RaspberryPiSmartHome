#include "thread_mqtt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "MQTTClient.h"		
#include "thread_ui.h"
#include "bsp_led.h"
#include "pcf8591.h"
#include "bsp_pwm.h"
#include "thread_ad.h"

#define BROKER_ADDRESS	"tcp://broker.emqx.io:1883"	//EMQX开源MQTT服务器地址

#define CLIENTID		"lqh123"		 //客户端id，随便写
#define USERNAME		""		         //用户名
#define PASSWORD		""			     //密码

#define WILL_TOPIC		"lqh/pi/will"		    //遗嘱主题

//接收到消息的回调函数
static int msgarrvd(void *context, char *topicName, int topicLen,
			MQTTClient_message *message)
{ 
	uint8_t temp_num = 0;
    printf("%16sReceived message on topic %s: %.*s\n","[thread_mqtt]:", topicName, (int)message->payloadlen, (char *)message->payload);
	if (!strcmp(topicName, "/lqh/pi/chufang")) {//校验消息的主题
		temp_num = atoi(message->payload);
		fflush ( stdout ) ;
		set_led_level(LED2,temp_num);
	}else if(!strcmp(topicName, "/lqh/pi/woshi"))
	{
		temp_num = atoi(message->payload);
		fflush ( stdout ) ;
		set_led_level(LED3,temp_num);
	}else if(!strcmp(topicName, "/lqh/pi/keting"))
	{
		temp_num = atoi(message->payload);
		fflush ( stdout ) ;
		set_led_level(LED4,temp_num);
	}else if(!strcmp(topicName, "/lqh/pi/yushi"))
	{
		temp_num = atoi(message->payload);
		fflush ( stdout ) ;
		set_led_level(LED5,temp_num);
	}else if(!strcmp(topicName, "/lqh/pi/menchuang"))
	{
		temp_num = atoi(message->payload);
		fflush ( stdout ) ;
		if(temp_num == 0) soft_pwm_set_count(PWM1,5);
		else if(temp_num == 1)  soft_pwm_set_count(PWM1,25);
	}else if(!strcmp(topicName, "/lqh/pi/dacled"))
	{
		temp_num = atoi(message->payload);
		fflush ( stdout ) ;
		mqtt_set_da_num(1,temp_num);
	}
	/* 释放占用的内存空间 */
	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	return 1;
}

//连接丢失的回调函数
static void connlost(void *context, char *cause)
{
	printf("\nConnection lost\n");
	printf("    cause: %s\n", cause);
}

void *thread_mqtt(void *parameter)
{
	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_willOptions will_opts = MQTTClient_willOptions_initializer;
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	int rc;

	/* 创建mqtt客户端对象 */
	if (MQTTCLIENT_SUCCESS !=
			(rc = MQTTClient_create(&client, BROKER_ADDRESS, CLIENTID,
			MQTTCLIENT_PERSISTENCE_NONE, NULL))) {
		printf("Failed to create client, return code %d\n", rc);
		rc = EXIT_FAILURE;
		goto destroy_exit;
	}

	/* 设置回调 */
	if (MQTTCLIENT_SUCCESS !=
			(rc = MQTTClient_setCallbacks(client, NULL, connlost,
			msgarrvd, NULL))) {
		printf("Failed to set callbacks, return code %d\n", rc);
		rc = EXIT_FAILURE;
		goto destroy_exit;
	}

	/* 连接MQTT服务器 */
	will_opts.topicName = WILL_TOPIC;	//遗嘱主题
	will_opts.message = "Unexpected disconnection";//遗嘱消息
	will_opts.retained = 1;	//保留消息
	will_opts.qos = 0;		//QoS0

	conn_opts.will = &will_opts;
	conn_opts.keepAliveInterval = 30;	//心跳包间隔时间
	conn_opts.cleansession = 0;			//cleanSession标志
	conn_opts.username = USERNAME;		//用户名
	conn_opts.password = PASSWORD;		//密码
	if (MQTTCLIENT_SUCCESS !=
			(rc = MQTTClient_connect(client, &conn_opts))) {
		printf("Failed to connect, return code %d\n", rc);
		rc = EXIT_FAILURE;
		goto destroy_exit;
	}

	printf("%16sMQTT服务器连接成功!\n","[thread_mqtt]:");

	/* 发布上线消息 */
	pubmsg.payload = "Online";	//消息的内容
	pubmsg.payloadlen = 6;		//内容的长度
	pubmsg.qos = 0;				//QoS等级
	pubmsg.retained = 1;		//保留消息
	if (MQTTCLIENT_SUCCESS !=
		(rc = MQTTClient_publishMessage(client, WILL_TOPIC, &pubmsg, NULL))) {
		printf("Failed to publish message, return code %d\n", rc);
		rc = EXIT_FAILURE;
	}

	/* 订阅主题 */
	if (MQTTCLIENT_SUCCESS !=
			(rc = MQTTClient_subscribe(client, "/lqh/pi/chufang", 0))) {
		printf("Failed to subscribe, return code %d\n", rc);
		rc = EXIT_FAILURE;
	}

	if (MQTTCLIENT_SUCCESS !=
			(rc = MQTTClient_subscribe(client, "/lqh/pi/woshi", 0))) {
		printf("Failed to subscribe, return code %d\n", rc);
		rc = EXIT_FAILURE;
	}

	if (MQTTCLIENT_SUCCESS !=
			(rc = MQTTClient_subscribe(client, "/lqh/pi/keting", 0))) {
		printf("Failed to subscribe, return code %d\n", rc);
		rc = EXIT_FAILURE;
	}

	if (MQTTCLIENT_SUCCESS !=
			(rc = MQTTClient_subscribe(client, "/lqh/pi/yushi", 0))) {
		printf("Failed to subscribe, return code %d\n", rc);
		rc = EXIT_FAILURE;
	}

	if (MQTTCLIENT_SUCCESS !=
			(rc = MQTTClient_subscribe(client, "/lqh/pi/menchuang", 0))) {
		printf("Failed to subscribe, return code %d\n", rc);
		rc = EXIT_FAILURE;
	}

	if (MQTTCLIENT_SUCCESS !=
			(rc = MQTTClient_subscribe(client, "/lqh/pi/dacled", 0))) {
		printf("Failed to subscribe, return code %d\n", rc);
		rc = EXIT_FAILURE;
	}

	/* 向服务端发布信息 */
	for ( ; ; ) {

		MQTTClient_message tempmsg = MQTTClient_message_initializer;
		char temp_str[10] = {0};
		char rh_str[10] = {0};
		char light_adc_str[10] = {0};
		ui_data_t ui_data_temp;
		int fd;

		ui_data_temp = get_ui_data();
        sprintf(temp_str,"%u",ui_data_temp.tmp);
		sprintf(rh_str,"%u",ui_data_temp.rh);
		sprintf(light_adc_str,"%u",ui_data_temp.light_adc);

		/* 发布温度信息 */
		tempmsg.payload = temp_str;	//消息的内容
		tempmsg.payloadlen = strlen(temp_str);		//内容的长度
		tempmsg.qos = 0;				//QoS等级
		tempmsg.retained = 1;		//保留消息
		if (MQTTCLIENT_SUCCESS !=
			(rc = MQTTClient_publishMessage(client, "/lqh/pi/tmp", &tempmsg, NULL))) {
			printf("Failed to publish message, return code %d\n", rc);
			rc = EXIT_FAILURE;
		}
		tempmsg.payload = rh_str;	//消息的内容
		tempmsg.payloadlen = strlen(rh_str);		//内容的长度
		tempmsg.qos = 0;				//QoS等级
		tempmsg.retained = 1;		//保留消息
		if (MQTTCLIENT_SUCCESS !=
			(rc = MQTTClient_publishMessage(client, "/lqh/pi/rh", &tempmsg, NULL))) {
			printf("Failed to publish message, return code %d\n", rc);
			rc = EXIT_FAILURE;
		}
		tempmsg.payload = light_adc_str;	//消息的内容
		tempmsg.payloadlen = strlen(light_adc_str);		//内容的长度
		tempmsg.qos = 0;				//QoS等级
		tempmsg.retained = 1;		//保留消息
		if (MQTTCLIENT_SUCCESS !=
			(rc = MQTTClient_publishMessage(client, "/lqh/pi/light", &tempmsg, NULL))) {
			printf("Failed to publish message, return code %d\n", rc);
			rc = EXIT_FAILURE;
		}
		sleep(10);		
	}
destroy_exit:
	MQTTClient_destroy(&client);
}


