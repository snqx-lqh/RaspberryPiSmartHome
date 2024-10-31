## UDP通信

这篇文章，将介绍如何在树莓派中使用Socket套接字进行UDP通信，分别会实现C语言版和Python版，按道理，这个应该只要是Linux系统，都支持实现。

如果想看socket相关函数的具体解析，可以去其他博客看，我这里只会按照自己的理解简要说明一下。

一般的编程流程就是

服务端：创建服务端套接字->绑定套接字->收发数据

客户端：创建客户端套接字->设置服务器IP相关数据->收发数据

## C语言实现

### 服务端

1、创建套接字

```c
int socket(int domain, int type, int protocol);	
```

- domain：即协议域，又称为协议族（family）。常用的协议族有，AF_INET（IPv4)、AF_INET6(IPv6)。
- type：指定socket类型。常用的socket类型有，SOCK_STREAM（流式套接字）、SOCK_DGRAM（数据报式套接字）
- protocol：就是指定协议。常用的协议有，IPPROTO_TCP、PPTOTO_UDP、IPPROTO_SCTP、IPPROTO_TIPC等，它们分别对应TCP传输协议、UDP传输协议、STCP传输协议、TIPC传输协议。当protocol为0时，会自动选择type类型对应的默认协议。

2、绑定套接字

```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

* sockfd：即socket描述字，它是通过socket()函数创建了，唯一标识一个socket。bind()函数就是将给这个描述字绑定一个名字。
* addr：一个const struct sockaddr *指针，指向要绑定给sockfd的协议地址。

3、接收数据

```c
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,struct sockaddr *src_addr, socklen_t *addrlen);
```

- sockfd：这是套接字的文件描述符，用于标识在哪个套接字上接收数据。

* buf：这是一个指向缓冲区的指针，用于存储接收到的数据。
* len：这是 `buf` 缓冲区的大小，以字节为单位。它指定了缓冲区可以接收的最大数据量。
* flag：这个参数目前对 `recvfrom` 函数而言几乎总是设置为 0
* src_addr：这是一个指向 `sockaddr` 结构的指针，用于存储发送数据报的源地址。
* addrlen：该变量在调用 `recvfrom` 之前应该被设置为 `src_addr` 指向的 `sockaddr` 结构的大小

- 返回值：`recvfrom` 成功时返回接收到的字节数。如果连接被对方正常关闭，则返回 0。如果发生错误，则返回 -1，并设置全局变量 `errno` 以指示错误原因。

4、发送数据

```c
ssize_t sendto(int sockfd, const void *buf, size_t len, int  flags,const struct sockaddr *dest_addr, socklen_t addrlen);
```

发送数据的时候需要指定发送的对象

完整代码

```c
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define  SERVER_IP   "192.168.3.18"
#define  SERVER_PORT 8888 

#define BUF_SIZE 1024

void Server_recvform_sendto(int fd)
{
	int byte = 0, cnt = 0;
	char buf[BUF_SIZE] = {0};
	socklen_t len = sizeof(struct sockaddr_in);
	struct sockaddr_in clientaddr;
	
	if(fd <= 0) 
	{
		perror("socket fd value err");
		return ;
	}
	
	while(1)
	{
		memset(buf, 0, BUF_SIZE );
        //读取client数据,有数据更新才读取，否则阻塞 clientaddr会把客户端连接时候的地址端口信息保存
		byte = recvfrom(fd, buf, BUF_SIZE, 0, (struct sockaddr *)&clientaddr, &len);	
        //客户端关闭时，读取数据个数为0
		if(byte == 0)							
		{
			printf("sockfd:%d read over\n", fd);
			break;
		}
		if(byte < 0)
		{
			perror("read failed");	
			break;
		}
		printf("client IP:%s, port:%d, datalen:%d, info:%s\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port, byte, buf );
		
		memset(buf, 0, BUF_SIZE );
		sprintf(buf, "server send cnt:%d\n", ++cnt);
        //往刚刚连接到自己的client发送信息
		sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&clientaddr, sizeof(clientaddr));
	}
	close(fd);
}

int main(int argc, void *argv[] )
{
	int listenfd;
	struct sockaddr_in serveraddr; //服务端IP地址信息
	
    //1、创建套接字
	listenfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(listenfd < 0)
	{
		perror("Create socket fail.");
		return -1;
	}	

	//2、绑定套接字
    memset( (void*)&serveraddr,0,sizeof(struct sockaddr_in) );
    serveraddr.sin_family 		= AF_INET;
	serveraddr.sin_port			= htons(SERVER_PORT);
	serveraddr.sin_addr.s_addr 	= inet_addr(SERVER_IP);
	
	if(bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr_in))<0)	//绑定
	{
		perror("bind error.");
		return -1;
	}
	//3、服务器接收和发送数据
	Server_recvform_sendto(listenfd);
	
	return 0;
}

```

然后编译这段代码

```bash
gcc -Wall server.c -o server  
```

 -Wall 表示编译时显示所有警告

编译完成后调用生成的server文件

```bash
sudo ./server
```

想要停止这个程序，`Ctrl+c`即可。

### 客户端

1、创建套接字

2、设置要连接的地址相关 

```c
struct sockaddr_in srvaddr;                //IPV4地址结构体
memset(&srvaddr, 0, sizeof(srvaddr));
srvaddr.sin_family = AF_INET;              
srvaddr.sin_port = htons(SERVER_PORT);           //端口号   5001~65535
srvaddr.sin_addr.s_addr = inet_addr(SERVER_IP);  //链接服务器的IP地址
```

主要是connect函数，他会连接到我们设定的服务器地址，UDP的Connect函数是可选的，如果使用了Connect，后面就可以像TCP一样使用write和read进行通信。

3、发送数据和读数据

这个和服务端处理一样。

完整代码

```c
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define BUF_SIZE     1024
#define SERVER_IP   "192.168.3.4"	//IP号之间不能有空格
#define SERVER_PORT 8888

void Client_recvfrom_sendto(int socketfd, struct sockaddr_in *addr)
{
	int byte = 0, cnt = 0;
	unsigned char data[BUF_SIZE];
	socklen_t len = sizeof(struct sockaddr_in);
	struct sockaddr_in serveraddr;
	
	while(1)
	{
		memset(data, 0, BUF_SIZE );
		sprintf(data, "client send data cnt:%d\n", ++cnt);
		//往服务器地址发送信息
		sendto(socketfd, data, strlen(data), 0, (struct sockaddr *)addr, sizeof(*addr));
		
		memset(data, 0, BUF_SIZE );
		//读取server数据,有数据更新才读取，否则阻塞
		byte = recvfrom(socketfd, data, BUF_SIZE, 0, (struct sockaddr *)&serveraddr, &len);
		if(byte == 0)
		{
			perror("read over");
			break;
		}
		if(byte < 0)
		{
			perror("read failed");
			break;
		}
		printf("server-->client datelen:%d info:%s\r\n",byte, data);	
 
	}	
	return;
}

int main(int argc, void *argv[] )
{
	int socketfd = -1;
	struct sockaddr_in servaddr;
	
	//1、创建套接字
	if( (socketfd = socket(AF_INET, SOCK_DGRAM, 0) ) ==  -1)	
	{
		perror("socket create failed!");
		return -1;
	}

	//2、设置要连接的服务器相关信息
	memset(&servaddr, 0, sizeof(servaddr) );
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	servaddr.sin_port = htons(SERVER_PORT);

	//3、服务端接收和发送信息
	Client_recvfrom_sendto(socketfd, &servaddr);
	return 0;	
}

```

然后编译这段代码

```bash
gcc -Wall client.c -o client  
```

 -Wall 表示编译时显示所有警告

编译完成后调用生成的client文件

```bash
sudo ./client
```

想要停止这个程序，`Ctrl+c`即可。

## Python语言实现

### 服务端

其实步骤和C语言的是一样的，都是

建立socket对象、绑定本地的IP地址、使用套接字收发

直接看代码注释即可

```python
#!/usr/bin/python3
# -*- coding: utf-8 -*-
import socket
 
def main():
    # udp 通信地址，服务器的IP+端口号
    udp_addr = ('192.168.17.129', 8888)
    # 创建socket实例 SOCK_DGRAM代表是UDP
    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    # 绑定端口
    udp_socket.bind(udp_addr)
    i = 0
    # 等待接收对方发送的数据
    while True:
        i = i + 1
        # 1024表示本次接收的最大字节数 recv_data 是接收到的数据 ip_port是这串数据的发送方IP和端口
        recv_data,ip_port = udp_socket.recvfrom(1024)  
        # 打印接收到的数据
        print("[From %s:%d]:%s" % (ip_port[0], ip_port[1], recv_data.decode("utf-8")))
        # 往发送方 ip_port 发送
        udp_socket.sendto(("Hello,I am a UDP socket for: " + str(i)) .encode('utf-8'),ip_port)
        print("send %d message" % i)
 
if __name__ == '__main__':
    print("udp server ")
    main()
 
```

### 客户端

客户端的流程就是建立socket对象、绑定服务器的IP地址、发送和接收数据

具体实现查看代码

```python
import socket
 
def main():
    # udp 通信地址，服务器的IP+端口号
    udp_addr = ('192.168.3.4', 8888)
    # 创建socket实例 SOCK_DGRAM代表是UDP
    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
 
    # 发送数据到指定的ip和端口
    for i in range(10):
        udp_socket.sendto(("Hello,I am a UDP socket for: " + str(i)) .encode('utf-8'), udp_addr)
        print("send %d message" % i)
        # 1024表示本次接收的最大字节数 recv_data 是接收到的数据 ip_port是这串数据的发送方IP和端口
        recv_data, ip_port = udp_socket.recvfrom(1024)   
        print(recv_data,ip_port)
    # 5. 关闭套接字
    udp_socket.close()
 
if __name__ == '__main__':
    print("udp client ")
    main()
```

