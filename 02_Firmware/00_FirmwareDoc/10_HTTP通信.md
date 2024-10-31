## HTTP通信

我这篇文章将会使用c语言下的curl库实现http的get和post，python语言下使用requests来实现get和post。

文中部分内容参考

https://openatomworkshop.csdn.net/6629c2a71a836825ed7b8370.html

## C语言实现

安装curl相关的库

```bash
sudo apt-get install libcurl4-openssl-dev
sudo apt-get install curl
```

关于测试，我是用的yapi创建的两个请求处理，可以去那个网站查看相关的处理，[官网](https://yapi.pro/)，我使用的不是很好，所以我暂时也不写教程了，可以用我下面的代码链接做测试。短时间应该不会改动。

### Curl处理相关库

一般处理流程是

初始化CURL库->获取CURL句柄用于本次传输->设置的传输配置选项->执行请求->处理响应->清理->全局清理

1、初始化Curl库，这个函数只能用一次。(调用curl_global_cleanup清理后可再次使用初始化)

```c
CURLcode curl_global_init(long flags);
```

* flags
   	CURL_GLOBAL_ALL: 初始化所有的 libcurl 功能。
   	CURL_GLOBAL_SSL: 初始化 SSL 相关的功能，如支持 HTTPS 议。
   	CURL_GLOBAL_WIN32: 在 Windows 平台上初始化一些特定的功能。
   	CURL_GLOBAL_NOTHING: 不做任何初始化，这种情况下需要手动初始化特定的功能。
* 返回值： CURLcode 类型的错误码，如果初始化成功则返回 CURLE_OK，否则返回其他错误码表示初始化失败

2、获取Curl句柄用于本次传输，每次传输都需要有一个句柄初始化

```c
CURL *curl = curl_easy_init();
```

3、设置的传输配置选项，设置多种配置，请求头、URL啥的

```c
curl_easy_setopt(handle, option, value)
```

例如

```c
struct curl_slist *headers = NULL;
headers = curl_slist_append(headers, "Content-Type: application/json");
curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);	//设置请求头
curl_easy_setopt(curl, CURLOPT_URL, "http://example.com");	//设置 URL
curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "key1=value1&key2=value2");	//设置请求体
curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // 设置请求超时时间,超时时间为10秒
curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);//设置写数据回调函数
```

4、执行请求，执行一次HTTP请求

```c
CURLcode res = curl_easy_perform(curl); 
```

5、处理响应

```c
if (res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
} else {    // 请求成功，处理响应数据  }
```

以下是常见的错误代码

```c
CURLE_OK (0): 操作成功完成。
CURLE_UNSUPPORTED_PROTOCOL (1): 不支持的协议。
CURLE_URL_MALFORMAT (3): URL 格式错误。
CURLE_COULDNT_RESOLVE_HOST (6): 无法解析主机名。
CURLE_COULDNT_CONNECT (7): 无法连接到主机或代理。
CURLE_OPERATION_TIMEDOUT (28): 操作超时。
CURLE_GOT_NOTHING (52): 未收到任何数据。
CURLE_SEND_ERROR (55): 发送数据时出错。
CURLE_RECV_ERROR (56): 接收数据时出错。
```

6、清理: 执行完HTTP请求后，需要清理资源，包括清理CURL句柄和释放libcurl相关的资源。`curl_easy_cleanup(curl);`

7、全局清理: 在程序结束前，需要对libcurl进行全局清理，释放相关资源。`curl_global_cleanup();`


### Get请求

知道了上面的流程，我们写一个GET的处理实例，处理实验代码如下

```c
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <curl/curl.h>  

//用于处理接收到的数据的回调函数
size_t GetDataWriteCallback(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    //将读到的数据放进标准输出打印出来
    //fwrite(ptr, size, nmemb, stdout); 
    char *str = (char*)ptr; 
    printf("%s\n",str);
    return size * nmemb; // 返回写入的字节数 
}

int GetUrlData(const char *url)
{
    CURL *curl;  
    CURLcode res;  
  
    curl = curl_easy_init();
    if(curl)
    {
        // 设置要访问的URL  
        curl_easy_setopt(curl, CURLOPT_URL, url);  
        // 禁用将HTTP头输出到标准输出（如果你需要的话，可以开启这个选项）  
        curl_easy_setopt(curl, CURLOPT_HEADER, 0L);  
        // 设置数据接收的回调函数  
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, GetDataWriteCallback);  
        // 执行CURL会话  
        res = curl_easy_perform(curl);  
        // 检查是否有错误发生  
        if (res != CURLE_OK) {  
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));  
            return 1;  
        }      
        // 清理  
        curl_easy_cleanup(curl);  
        return 0; 
    }
    return 1;
}

int main(int argc,char **argv)
{
    curl_global_init(CURL_GLOBAL_ALL);

    const char *GetUrl = "https://yapi.pro/mock/194105/emp/list"; 
    if (!GetUrlData(GetUrl)) {  
        printf("Data fetched successfully.\n");  
    } else {  
        printf("Failed to fetch data.\n");  
    } 

    curl_global_cleanup(); 
    return 0; 
}
```

然后编译这段代码

```bash
gcc -Wall main.c -o main  
```

 -Wall 表示编译时显示所有警告

编译完成后调用生成的main文件

```bash
sudo ./main
```

想要停止这个程序，`Ctrl+c`即可。

### Post请求

知道了上面的流程，我们写一个POST的处理实例，处理实验代码如下

```c
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <curl/curl.h>  

size_t PostDataWriteCallback(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    //将读到的数据放进标准输出打印出来
    //fwrite(ptr, size, nmemb, stdout); 
    char *str = (char*)ptr; 
    printf("%s\n",str);
    return size * nmemb; // 返回写入的字节数 
}

int PostUrlData(const char *url, const char *json_data)
{
    CURL *curl;  
    CURLcode res;  
  
    curl = curl_easy_init();  
    if (curl) {  
        // 设置要访问的URL  
        curl_easy_setopt(curl, CURLOPT_URL, url);  
        // 设置POST请求  
        curl_easy_setopt(curl, CURLOPT_POST, 1L);  
        // 设置POST数据  
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);  
        // 设置Content-Type头  
        struct curl_slist *headers = NULL;  
        headers = curl_slist_append(headers, "Content-Type: application/json");  
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);  
        // 设置数据接收的回调函数（对于POST请求，这通常不是必需的）  
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, PostDataWriteCallback);  
        // 执行CURL会话  
        res = curl_easy_perform(curl);  
        // 检查是否有错误发生  
        if (res != CURLE_OK) {  
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));  
            curl_slist_free_all(headers); // 释放HTTP头  
            curl_easy_cleanup(curl);  
            return 1;  
        }  
        // 释放HTTP头  
        curl_slist_free_all(headers);  
  
        // 清理  
        curl_easy_cleanup(curl);  
        return 0;  
    }  
  
    // 如果curl初始化失败  
    return 1;  
}

int main(int argc,char **argv)
{
    curl_global_init(CURL_GLOBAL_ALL);

    const char *PostUrl = "https://yapi.pro/mock/194105/postData";
    const char *json_data = "{\"code\":\"13\"}"; // 示例JSON字符串
    if (!PostUrlData(PostUrl,json_data)) {  
        printf("JSON data posted successfully.\n");  
    } else {  
        printf("Failed to post JSON data.\n");  
    } 

    curl_global_cleanup(); 
    return 0; 
}
```

然后编译这段代码

```bash
gcc -Wall main.c -o main  
```

 -Wall 表示编译时显示所有警告

编译完成后调用生成的main文件

```bash
sudo ./main
```

想要停止这个程序，`Ctrl+c`即可。

## Python实现

这里的使用，还是使用的[第6节笔记](https://blog.csdn.net/wan1234512/article/details/141300460)建立的虚拟环境，如果不想看那节也可以自己再建一个

我们使用python安装的时候，应该会报错error: externally-managed-environment，网上有几种方法解决，可以参考[这篇博文](https://blog.csdn.net/2202_75762088/article/details/134625775)，我使用的是他说的创建venv的方式。

```bash
python -m venv ~/myenv        #创建虚拟环境，myenv就是环境名
source ~/myenv/bin/activate   #使能我们创建的虚拟环境
pip install luma.oled         #再次Pip安装
```

其他env操作

```
deactivate #退出环境
```

然后使用的是requests来实现get和post

```
pip install requests
```

### Get请求

```python
import requests  
  
url = 'https://yapi.pro/mock/194105/emp/list'  
response = requests.get(url)  
print(response.status_code)  # 打印响应状态码  
print(response.text)         # 打印响应内容（字符串形式）
```

### Post请求

```python
import requests 

url = 'https://yapi.pro/mock/194105/postData'  
data = {'code': '12'}  
response = requests.post(url, data=data)  
print(response.json())  # 如果响应是JSON格式，则可以直接解析
```

