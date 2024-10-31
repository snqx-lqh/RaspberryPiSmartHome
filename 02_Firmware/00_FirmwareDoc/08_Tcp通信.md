## TCP通信

这篇文章，将介绍如何在树莓派中使用Socket套接字进行TCP通信，分别会实现C语言版和Python版，按道理，这个应该只要是Linux系统，都支持实现。

如果想看socket相关函数的具体解析，可以去其他博客看，我这里只会按照自己的理解简要说明一下。

一般的编程流程就是

服务端：创建服务端套接字->绑定套接字->设置监听->接收客户端连接并生成新的套接字->收发数据

客户端：创建客户端套接字->连接服务器->收发数据

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

3、设置监听

```c
int listen(int sockfd, int backlog);
```

* sockfd：第一个参数即为要监听的socket描述字
* backlog：第二个参数为相应socket可以排队的最大连接个数。

4、接收客户端的连接，并生成新的套接字。

```c
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

* sockfd：第一个参数为服务器的socket描述字
* backlog：第二个参数为指向struct sockaddr *的指针，用于返回客户端的协议地址
* addrlen：第三个参数为客户端协议地址的长度。
* 如果accpet成功，那么其返回值是由内核自动生成的一个全新的描述字，代表与返回客户的TCP连接。

5、接收数据

```c
ssize_t read(int fd, void *buf, size_t count);
```

* fd：第一个参数为服务器与客户端连接生成的socket描述字
* buf：第二个参数为读取数据的存放位置
* count：第三个参数为数据的长度。

6、发送数据

```c
ssize_t write(int fd, const void *buf, size_t count);
```

* fd：第一个参数为服务器与客户端连接生成的socket描述字
* buf：第二个参数为发送数据的存放位置
* count：第三个参数为数据的长度。

完整代码

```c
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
			
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

 
#define  SERVER_IP   "192.168.17.129"
#define  SERVER_PORT 8888 

int main()
{
    //接收数据的Buff
	char RecBuf[1024];

	//1创建套接字
	int sockfd = socket(AF_INET,SOCK_STREAM,0);		 
	if(sockfd == -1)
	{
		perror("socket");
		return -1;	
	}
	
	//2绑定套接字
	struct sockaddr_in myaddr;                //IPV4地址结构体
	memset(&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;              
	myaddr.sin_port = htons(SERVER_PORT);           //端口号   5001~65535
	myaddr.sin_addr.s_addr = inet_addr(SERVER_IP);  //设置IP地址
	
	if (-1 == bind(sockfd, (struct sockaddr*)&myaddr, sizeof(myaddr))) {
		perror("bind");	
		return -1;
	}
	
	//3设置监听套接字
	if(listen(sockfd,5) == -1)
	{
		perror("listen");
		return -1;	//返回现在执行的函数(结束函数) 
	}
	
	while(1)
	{
        //4接受客户端的连接。并生成新的通信套接字
        int connfd = accept(sockfd,NULL,NULL);			//定义套接字
        if(connfd == -1)
        {
            perror("accpet!");
            return -1;	//返回现在执行的函数(结束函	
        }
        printf("accpet success!\n");
        //连接成功后，进入通信的循环
        while(1)
        {
            memset(RecBuf,0,sizeof(RecBuf));		//将 buf空间中的1024内容清空 
            int ret = read(connfd,RecBuf,sizeof(RecBuf));
            if(ret == -1)			//读取失败 
            {	
                perror("read!");
                break;
            }
            else if(ret == 0)		//写端口关闭 
            {
                printf("client close\n");
                break;
            }
            else
            {
                printf("recv : %s\n",RecBuf);
            } 
            if (0 == strncmp(RecBuf, "exit",4))	//判断是否退出
                break;
            
            //将读取到的数据又发送出来
            ret = write(connfd, RecBuf, sizeof(RecBuf));
            if (ret == -1) {
                perror("write");
                break;
            }
        }
		//关闭连接套接字 
	    close(connfd);
	}	
	close(sockfd);	
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

```c
int socket(int domain, int type, int protocol);	
```

这部分和服务端一样

2、设置要连接的地址相关并连接服务器

```c
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

主要是connect函数，他会连接到我们设定的服务器地址

- 第一个参数即为客户端的socket描述字，
- 第二参数为服务器的socket地址，
- 第三个参数为socket地址的长度。

3、发送数据和读数据

这个和服务端处理一样。

完整代码

```c
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
			
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

 
#define  SERVER_IP   "192.168.3.18"
#define  SERVER_PORT 8888 

int main()
{
    //接收数据的Buff
	char RecBuf[1024];

	//1创建套接字
	int sockfd = socket(AF_INET,SOCK_STREAM,0);		 
	if(sockfd == -1)
	{
		perror("socket");
		return -1;	
	}
	
	//2绑定套接字
	struct sockaddr_in myaddr;                //IPV4地址结构体
	memset(&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;              
	myaddr.sin_port = htons(SERVER_PORT);           //端口号   5001~65535
	myaddr.sin_addr.s_addr = inet_addr(SERVER_IP);  //设置IP地址
	
	if (-1 == bind(sockfd, (struct sockaddr*)&myaddr, sizeof(myaddr))) {
		perror("bind");	
		return -1;
	}
	
	//3设置监听套接字
	if(listen(sockfd,5) == -1)
	{
		perror("listen");
		return -1;	//返回现在执行的函数(结束函数) 
	}
	
	while(1)
	{
        //4接受客户端的连接。并生成新的通信套接字
        int connfd = accept(sockfd,NULL,NULL);			//定义套接字
        if(connfd == -1)
        {
            perror("accpet!");
            return -1;	//返回现在执行的函数(结束函	
        }
        printf("accpet success!\n");
        //连接成功后，进入通信的循环
        while(1)
        {
            memset(RecBuf,0,sizeof(RecBuf));		//将 buf空间中的1024内容清空 
            int ret = read(connfd,RecBuf,sizeof(RecBuf));
            if(ret == -1)			//读取失败 
            {	
                perror("read!");
                break;
            }
            else if(ret == 0)		//写端口关闭 
            {
                printf("client close\n");
                break;
            }
            else
            {
                printf("recv : %s\n",RecBuf);
            } 
            if (0 == strncmp(RecBuf, "exit",4))	//判断是否退出
                break;
            
            //将读取到的数据又发送出来
            ret = write(connfd, RecBuf, sizeof(RecBuf));
            if (ret == -1) {
                perror("write");
                break;
            }
        }
		//关闭连接套接字 
	    close(connfd);
	}	
	close(sockfd);	
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

建立socket对象、绑定本地的IP地址、设置监听的最大连接数、阻塞等待连接、连接好了之后就使用连接好的套接字收发

直接看代码注释即可

```python
"""
Socket 服务器 代码示例
"""
import socket

#创建socket实例对象
socket_server = socket.socket()

#为实例对象 绑定服务端的 IP 地址和端口号
socket_server.bind(("192.168.3.18", 8888))

#设置监听的最大连接数
socket_server.listen(100)

while True:
    #阻塞等待连接 , 函数返回连接的客户端 socket 对象 和 连接的地址
    client_socket, client_address = socket_server.accept()

    #服务器端与客户端进行交互
    while True:
        # 循环接收客户端数据, 1024是最大长度，并使用 UTF-8 解码
        data = client_socket.recv(1024).decode("UTF-8")

        # 向客户端发送接收到的消息
        client_socket.send(f"server rec: {data}".encode())
        print(f"client send: {data}")

        if data == 'quit':
            break

    # 关闭连接
    client_socket.close()
    print(f'client disconnect {client_address}')
```

### 客户端

客户端的流程就是建立socket对象、绑定服务器的IP地址、发送和接收数据

具体实现查看代码

```python
import socket

#  创建 socket 实例对象
client_socket = socket.socket()

#  客户端连接服务器, IP 地址和端口号放在元组中
client_socket.connect(('192.168.3.4', 8888))

# 获取命令行输入发送给客户端
while True:
    command = input("请输入: ")
    client_socket.send(command.encode())
    print(f"客户端发送: {command}")
    if command == 'quit':
        break

    # 接收服务器数据
    data = client_socket.recv(1024).decode("UTF-8")
    print(f"服务端: {data}")

#关闭连接
client_socket.close()
print("客户端关闭")
```

