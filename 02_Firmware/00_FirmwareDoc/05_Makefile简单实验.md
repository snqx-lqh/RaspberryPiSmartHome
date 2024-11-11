## 说明

我们前面分文件编写后，编译需要编写的指令长度越来越长，后面我们有更多的文件后，编译的文件也会更多，所以我们需要使用一个编译管理的工具Makefile，但是这一部分，我们不介绍过多关于Makefile的东西，只是简单介绍并且做一个应用。

## Makefile基础

我们可以先查看一下我们树莓派的make版本，下面是我的打印信息，有版本，许可等。

```bash
pi@raspberrypi:~ $ make --version
GNU Make 4.3
为 arm-unknown-linux-gnueabihf 编译
Copyright (C) 1988-2020 Free Software Foundation, Inc.
许可证：GPLv3+：GNU 通用公共许可证第 3 版或更新版本<http://gnu.org/licenses/gpl.html>。
本软件是自由软件：您可以自由修改和重新发布它。
在法律允许的范围内没有其他保证。
```

建立一个Makefile文件，我们先写一个简易的文件，Makefile就是一个编译和链接多个文件的工具，下面这个就是一个简单的例子，hello是目标文件，hello.cpp是依赖，再另起一行的就是怎么使用这个依赖生成目标。

```makefile
main: main.c bsp_led.c bsp_key.c bsp_beep.c
	cc -Wall -o main main.c bsp_led.c bsp_key.c bsp_beep.c -lwiringPi # 开头必须为一个Tab，不能为空格
```

然后使用的话，make，执行生成文件即可。

```makefile
make 
```

### 最终目标

Makefile中会有很多目标，但最终目标只有一个，写Makefile的时候**先写出最终目标，再依次解决总目标的依赖**。执行指令make，即可编译。

```makefile
make 
```

### 伪目标

如果我们文件目录下有一个文件和我们的执行目标重名，就会冲突，比如我们的Makefile文件中有clean，如果文件夹中还有clean就会冲突。这时候我们一般会给这个目标加一个标识`.PHONY`，设置为伪目标，就不会冲突了。

```makefile
.PHONY: clean
clean:
	rm main
```

### 依赖检查

每次使用make的时候，他就会去检查依赖是否有更新，然后更新有变化的依赖，我们不想每次把所有的文件更新，就会只是让main依赖.o文件，如果.o文件有更新就先更新，更新完成后再更新main。

```makefile
main: main.o bsp_led.o bsp_key.o bsp_beep.o 
	cc -o main main.o bsp_led.o bsp_key.o bsp_beep.o -Wall -lwiringPi 

main.o: main.c 
	cc -c main.c -Wall -lwiringPi

bsp_led.o: bsp_led.c  
	cc -c bsp_led.c -Wall -lwiringPi 

bsp_key.o: bsp_key.c 
	cc -c bsp_key.c -Wall -lwiringPi 

bsp_beep.o: bsp_beep.c 
	cc -c bsp_beep.c -Wall -lwiringPi

.PHONY: clean
clean:
	rm main.o bsp_led.o bsp_key.o bsp_beep.o 
	rm main
```

## 变量

把每个.o文件都列出来，显得比较臃肿，我们就会想着用一个变量去替代他，makefile中的变量都是字符串，变量定义好好后用$使用。比如我这里定义了变量OBJ、LDLIBS、CFLAGS、CC。

```makefile
OBJ = main.o bsp_led.o bsp_key.o bsp_beep.o 
CFLAGS	= -Wall
LDLIBS  = -lwiringPi
CC	    = cc

main: $(OBJ)
	$(CC) -o main $(OBJ) $(CFLAGS) $(LDLIBS)

main.o: main.c 
	$(CC) -c main.c $(CFLAGS) $(LDLIBS)

bsp_led.o: bsp_led.c  
	$(CC) -c bsp_led.c $(CFLAGS) $(LDLIBS)

bsp_key.o: bsp_key.c 
	$(CC) -c bsp_key.c $(CFLAGS) $(LDLIBS)

bsp_beep.o: bsp_beep.c 
	$(CC) -c bsp_beep.c $(CFLAGS) $(LDLIBS)

.PHONY: clean
clean:
	rm $(OBJ) 
	rm main
```

## 自动变量

上面的Makefile文件，看着还是很长，makefile中有许多自动变量可以批量替换文件

```
$@：①本条规则的目标名；②如果目标是归档文件的成员，则为归档文件名；③在多目标的模式规则中, 为导致本条规则方法执行的那个目标名；
$<：本条规则的第一个依赖名称
$?：依赖中修改时间晚于目标文件修改时间的所有文件名，以空格隔开
$^：所有依赖文件名，文件名不会重复，不包含order-only依赖
$+：类似上一个， 表示所有依赖文件名，包括重复的文件名，不包含order-only依赖
$|：所有order-only依赖文件名
$*：(简单理解)目标文件名的主干部分(即不包括后缀名)
$%：如果目标不是归档文件，则为空；如果目标是归档文件成员，则为对应的成员文件名
```

根据这个规则，我们可以将上面的文件修改为以下，主要使用$@和$<。分别代表规则目标名和依赖的第一个名

```makefile
OBJ = main.o bsp_led.o bsp_key.o bsp_beep.o 
CFLAGS	= -Wall
LDLIBS  = -lwiringPi
CC	    = cc

main: $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS) $(LDLIBS)

main.o: main.c 
	$(CC) -c $< $(CFLAGS) $(LDLIBS)

bsp_led.o: bsp_led.c  
	$(CC) -c $< $(CFLAGS) $(LDLIBS)

bsp_key.o: bsp_key.c 
	$(CC) -c $< $(CFLAGS) $(LDLIBS)

bsp_beep.o: bsp_beep.c 
	$(CC) -c $< $(CFLAGS) $(LDLIBS)

.PHONY: clean
clean:
	rm $(OBJ) 
	rm main
```

## 批量替换

会发现我们有很多的.o和.h文件，可以使用批量替换，直接使用%.c和%.o，他就会把每一个符合这个后缀的进行编译。

```makefile
OBJ = main.o bsp_led.o bsp_key.o bsp_beep.o 
CFLAGS	= -Wall
LDLIBS  = -lwiringPi
CC	    = cc

main: $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS) $(LDLIBS)

%.o: %.c 
	$(CC) -c $< $(CFLAGS) $(LDLIBS)

.PHONY: clean
clean:
	rm $(OBJ) 
	rm main
```

## 生成文件另存

大家会发现上面这样生成的文件，会生成一堆的.o文件，然后和c文件又混合在一起，不太好看，我们就把生成的.o文件另存。

首先我们使用两个变量O_PATH_DIR和SUBDIR存放我们想要存放.o文件的位置。

然后SOURCE_PATH使用通配符把目录下所有的.c文件获得。

使用`foreach`是makefile的一个函数，`$(foreach var,list,text)` ，这是一个循环遍历函数。它会遍历 `list` 中的每个元素，对于每个元素，将其赋值给 `var`，然后对 `text` 进行求值。在这个语句中，`var` 是 `dir`，`list` 是 `$(SUBDIR)`，`text` 是`$(wildcard $(dir)/*.c)`。`$(wildcard pattern)` 函数用于获取匹配指定 `pattern` 的文件列表。`pattern` 可以包含通配符，比如`*`（匹配任意字符序列）和`?`（匹配单个任意字符）等。这里的`$(wildcard $(dir)/*.c)`表示在当前 `dir` 目录下查找所有后缀为 `.c` 的文件。

使用`patsubst`是makefile的一个函数，它可以把字符串中的能配上的规则替换，这里的`%.c,$(O_PATH_DIR)/%.o`，就是把字符串中的.c结尾替换成.o结尾。这个字符串从哪里来呢，就是由`$(notdir $(SOURCE_PATH))`这个方法提供，他会把`SOURCE_PATH`下的文件，不加路径名，只有文件名，并转成字符串提供给前面。

后面的处理，和之前相比，就是要替换依赖的路径，还有就是`$(O_PATH_DIR)/%.o: $(SUBDIR)/%.c `，这里的处理需要加上路径，以及他们的执行生成需要加上.o文件的保存位置，即在处理后加上`-o $@`，生成.o文件的路径，就是目标名路径。

```Makefile
#.o文件存放路径
O_PATH_DIR = ./build
#源文件路径
SUBDIR = .

SOURCE_PATH = $(foreach dir,$(SUBDIR),$(wildcard $(dir)/*.c))
OBJ_PATH    = $(patsubst %.c,$(O_PATH_DIR)/%.o, $(notdir $(SOURCE_PATH)))


CFLAGS	= -Wall
LDLIBS  = -lwiringPi
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

这样处理后，我们在编程前，需要先建立一个build文件，来保存我们的.o文件。

```makefile
mkdir build
make
```

## 总结

这样的一个makefile文件，就可以支撑我们每次的编译实现了，每次只需要在这个文件里面去修改目标文件需要的.o文件就可以了。后面项目更大后我们将其他的处理。

