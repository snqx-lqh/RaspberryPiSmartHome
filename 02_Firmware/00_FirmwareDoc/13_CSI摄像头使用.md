## 说明

这篇文章说一下树莓派CSI摄像头的使用方法，如果使用的是我这一版镜像，默认CSI摄像头是使能了的，只需要连接好摄像头后reboot即可。

## 测试

重启后，使用以下指令

```bash
ls /dev/video*
```

如果查看到video0设备，即是检测到了设备，可以在树莓派终端下输入以下指令测试树莓派的摄像头。

```bash
#测试摄像头
libcamera-hello 
#拍照
libcamera-jpeg -o test.jpg
```

但是呢，你使用这个，就不能使用opencv来掉摄像头了。如果要能用opencv掉摄像头

修改config.txt文件 输入

```bash
sudo nano /boot/firmware/config.txt
```

在文件最后加上如下命令：加在最后【all】

```bash
gpu_mem=128
start_x=1
```

 注释掉原来的摄像头自动检测语句 

```bash
#camera_auto_detect=1
```

  Ctrl+o 写入  Ctrl+x 退出

修改/etc/modules输入

```bash
sudo nano /etc/modules
```

在最后面添加如下命令

```bash
bcm2835-v4l2
```


Ctrl+o 写入  Ctrl+x 退出

保存后，重启系统！！！验证

```bash
vcgencmd get_camera
```

得到 supported=1 detected=1，则说明摄像头可以工作了

带来的问题  修改后输入：

```bash
libcamera-hello
```

命令出现“no cameras available”报错，就是原来的又不行了。网上有位网友说pios把摄像头驱动更换到了libcamera，而opencv当前还不支持libcamera，是你opencv不能用的根本原因。最终解决的办法就是回滚到老版的v4l2驱动，opencv支持v4l2，所以就ok了。这种方法是退回到老的驱动来实现opencv的读取。



下面将分别使用C++和Python版本的opencv调用这个摄像头。

## C++版

首先我们先安装一些opencv库，这个是编译好的opencv，如果自己在树莓派上编译花的时间太久了

```bash
sudo apt-get install libopencv-dev   libopencv-contrib-dev
```

然后写一个实例，这个只是一个打开摄像头

```c
#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;  
using namespace cv;  
  
int main(int argc, char** argv)  
{  
    // 打开视频文件  
    VideoCapture cap(0);  
  
    // 检查是否成功打开视频文件  
    if (!cap.isOpened())  
    {  
        std::cerr << "无法打开视频文件" << std::endl;  
        return -1;  
    }  
  
    // 设置窗口大小  
    namedWindow("Video", WINDOW_NORMAL);  
    resizeWindow("Video", 400, 400);  
  
    // 循环播放视频直到按下退出键  
    while (true)  
    {  
        Mat frame;  
  
        // 从视频文件中读取下一帧  
        cap >> frame;  
  
        // 如果读取的帧为空，则视频播放结束，退出循环  
        if (frame.empty())  
            break;  
  
        // 在窗口中显示当前帧  
        imshow("Video", frame);  
  
        // 等待一段时间，然后继续循环（按Q键退出）  
        if (waitKey(30) == 'q' || waitKey(30) == 27) // 'q'键或Esc键  
            break;  
    }  
  
    // 释放视频文件和窗口资源  
    cap.release();  
    destroyAllWindows();  
  
    return 0;  
}
```

然后我们使用cmake来构建makefile，关于cmake的知识，可以去看其他博主的讲解，很多比较详细

```cmake
cmake_minimum_required(VERSION 3.16.3)
project(main)
set(CMAKE_CXX_STANDARD 14)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_DIRCTORIES})
add_executable(main main.cpp)
target_link_libraries(main ${OpenCV_LIBRARIES})
```

然后使用以下指令执行

```bash
cmake .
make
./main
```

第一次会比较慢，要等一会。

## Python版

安装opencv库，但是首先先建立虚拟环境，如果你有虚拟环境就不用管这一步

```bash
python -m venv ~/myenv        #创建虚拟环境，myenv就是环境名
source ~/myenv/bin/activate   #使能我们创建的虚拟环境
```

其他env操作

```
deactivate #退出环境
```

安装opencv相关

```bash
pip3 install opencv-contrib-python -i https://pypi.tuna.tsinghua.edu.cn/simple
pip3 install opencv-python -i https://pypi.tuna.tsinghua.edu.cn/simple
```

写一个简单的调用摄像头的代码

```python
import cv2
 
capture = cv2.VideoCapture(0)  
while (capture.isOpened()):  
    retval, image = capture.read()  
    cv2.imshow("Video", image)  
    key = cv2.waitKey(1)  
    if key == 32:  
        break
        
capture.release() 
cv2.destroyAllWindows()  
```

