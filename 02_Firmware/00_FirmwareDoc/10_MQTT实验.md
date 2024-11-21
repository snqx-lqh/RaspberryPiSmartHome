## MQTT实验

我们的智能家居项目，将使用MQTT发布和订阅状态，所以我们需要了解mqtt的使用方式。

使用的免费服务器是EMQX的https://www.emqx.com/zh/mqtt/public-mqtt5-broker

使用的在线客户端也是他们公司的MQTTXhttps://mqttx.app/zh

## C语言实现

### 环境搭建

去mqtt的官网下载一个客户端源码[git_releases](https://github.com/eclipse/paho.mqtt.c/releases)，我这里下载的版本是1.3.13。

安装依赖

```bash
sudo apt-get install libssl-dev
```

然后编译源码

```bash
tar -zxvf paho.mqtt.c-1.3.13.tar.gz
cd paho.mqtt.c-1.3.13
make
sudo make install
```

完成后

```bash
ls /usr/local/lib
```

就可以看到有相关的链接文件了。

### 代码编写

代码编写主要参考正点原子的Linux中的MQTT实验。

流程如下

1、创建MQTT客户端对象

```c
int MQTTClient_create(MQTTClient* handle, const char* serverURI, const char* clientId,
		int persistence_type, void* persistence_context);
```

* handle：mqtt客户端的句柄。
* serverURI：服务器的地址和端口
* clientId：客户端ID
* persistence_type：这是一个整数参数，用于指定 MQTT 客户端如何处理持久化。
* persistence_context：这是一个通用指针，可以指向与持久化相关的特定上下文信息。

2、设置回调函数

```c
int MQTTClient_setCallbacks(MQTTClient handle, void* context,MQTTClient_connectionLost* cl,MQTTClient_messageArrived* ma, MQTTClient_deliveryComplete* dc);
```

* handle：mqtt客户端的句柄。
* context：这是一个通用指针，可以指向任何用户定义的上下文信息。在回调函数中，可以通过这个指针获取与特定应用相关的额外数据或状态。通常可以将其设置为指向包含应用特定状态的结构体或对象的指针。
* cl：该函数通常用于通知应用程序连接丢失，并采取适当的措施，例如尝试重新连接。
* ma：这是一个指向函数的指针，用于处理接收到的 MQTT 消息。
* dc：这是一个指向函数的指针，用于处理消息的发送完成事件。

3、连接MQTT服务器

```c
int MQTTClient_connect(MQTTClient handle, MQTTClient_connectOptions* options);
```

* handle：mqtt客户端的句柄。
* options：设置遗嘱、用户名、心跳等信息

4、发布消息

```c
int MQTTClient_publishMessage(MQTTClient handle, const char* topicName, MQTTClient_message* msg, MQTTClient_deliveryToken* dt);
```

* handle：mqtt客户端的句柄。
* topicName：主题名
* msg：消息相关信息，如payload、payloadlen、qos、retained。
* dt：可以通过这个令牌来检查消息是否成功发送。如果不需要跟踪消息的发送状态，可以将这个参数设置为`NULL`。

5、订阅消息

```c
int MQTTClient_subscribe(MQTTClient handle, const char* topic, int qos);
```

* handle：mqtt客户端的句柄。
* topic：主题名
* qos：服务质量等级



代码是在正点原子的开源代码基础上修改的，他是使用的然也，但是然也已经没了。这里使用的是EMQX的开源地址。在修改这部分代码的时候遇到的**坑**是，最开始我在接收消息的回调函数里面写printf的时候，没有加`\n`，导致回调这里一直出问题，不能打印。

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "MQTTClient.h"		


#define BROKER_ADDRESS	"tcp://broker.emqx.io:1883"	//EMQX开源MQTT服务器地址

#define CLIENTID		"lqh123"		 //客户端id，随便写
#define USERNAME		""		         //用户名
#define PASSWORD		""			     //密码

#define WILL_TOPIC		"lqh/pi/will"		    //遗嘱主题
#define LED_TOPIC		"lqh/pi/led"		    //LED主题
#define TEMP_TOPIC		"lqh/pi/temperature"	//温度主题

//接收到消息的回调函数
static int msgarrvd(void *context, char *topicName, int topicLen,
			MQTTClient_message *message)
{ 
    printf("Received message on topic %s: %.*s\n", topicName, (int)message->payloadlen, (char *)message->payload);
	if (!strcmp(topicName, LED_TOPIC)) {//校验消息的主题
		if (!strcmp("2", message->payload))	
			printf("receive 2\n");
		if (!strcmp("1", message->payload)) {
			printf("receive 1\n");
		}
		else if (!strcmp("0", message->payload)) {
			printf("receive 0\n");
		}
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

int main(int argc, char *argv[])
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
		goto exit;
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

	printf("MQTT服务器连接成功!\n");

	/* 发布上线消息 */
	pubmsg.payload = "Online";	//消息的内容
	pubmsg.payloadlen = 6;		//内容的长度
	pubmsg.qos = 0;				//QoS等级
	pubmsg.retained = 1;		//保留消息
	if (MQTTCLIENT_SUCCESS !=
		(rc = MQTTClient_publishMessage(client, WILL_TOPIC, &pubmsg, NULL))) {
		printf("Failed to publish message, return code %d\n", rc);
		rc = EXIT_FAILURE;
		goto disconnect_exit;
	}

	/* 订阅主题 dt_mqtt/led */
	if (MQTTCLIENT_SUCCESS !=
			(rc = MQTTClient_subscribe(client, LED_TOPIC, 0))) {
		printf("Failed to subscribe, return code %d\n", rc);
		rc = EXIT_FAILURE;
		goto disconnect_exit;
	}

	/* 向服务端发布芯片温度信息 */
	for ( ; ; ) {

		MQTTClient_message tempmsg = MQTTClient_message_initializer;
		char temp_str[10] = {0};
		int fd;

        sprintf(temp_str,"123");

		/* 发布温度信息 */
		tempmsg.payload = temp_str;	//消息的内容
		tempmsg.payloadlen = strlen(temp_str);		//内容的长度
		tempmsg.qos = 0;				//QoS等级
		tempmsg.retained = 1;		//保留消息
		if (MQTTCLIENT_SUCCESS !=
			(rc = MQTTClient_publishMessage(client, TEMP_TOPIC, &tempmsg, NULL))) {
			printf("Failed to publish message, return code %d\n", rc);
			rc = EXIT_FAILURE;
			goto unsubscribe_exit;
		}

		sleep(30);		//每隔30秒 更新一次数据
	}

unsubscribe_exit:
	if (MQTTCLIENT_SUCCESS !=
		(rc = MQTTClient_unsubscribe(client, LED_TOPIC))) {
		printf("Failed to unsubscribe, return code %d\n", rc);
		rc = EXIT_FAILURE;
	}
disconnect_exit:
	if (MQTTCLIENT_SUCCESS !=
		(rc = MQTTClient_disconnect(client, 10000))) {
		printf("Failed to disconnect, return code %d\n", rc);
		rc = EXIT_FAILURE;
	}
destroy_exit:
	MQTTClient_destroy(&client);
exit:
	return rc;
}

```

编写完成后使用之前的Makefile文件，但是有一点改进，需要在LDLIBS变量后面加一个mqtt库`-lpaho-mqtt3c`。

```makefile
#.o文件存放路径
O_PATH_DIR = ./build
#源文件路径
SUBDIR = .

SOURCE_PATH = $(foreach dir,$(SUBDIR),$(wildcard $(dir)/*.c))
OBJ_PATH    = $(patsubst %.c,$(O_PATH_DIR)/%.o, $(notdir $(SOURCE_PATH)))

CFLAGS	= -Wall
LDLIBS  = -lwiringPi -lpaho-mqtt3c
CC	    = cc

main:  $(OBJ_PATH)
	$(CC) -o $@  $(OBJ_PATH) $(CFLAGS) $(LDLIBS)

$(O_PATH_DIR)/%.o: $(SUBDIR)/%.c 
	$(CC) -c $< $(CFLAGS) $(LDLIBS) -o $@

.PHONY: clean
clean:
	rm $(OBJ_PATH) 
	rm main
```

make即可。

```bash
make
sudo ./main
```

想要停止这个程序，`Ctrl+c`即可。代码测试可以使用手机上的MQTT小程序发布订阅，也可以用他的电脑客户端软件MQTTX。

