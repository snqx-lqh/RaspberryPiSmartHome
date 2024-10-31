## 说明

这篇笔记将简要介绍Makefile的使用方式并且记录一些常用的makefile相关函数。主要使用还是在树莓派上面实现。

本文主要是看的B站[从零开始学Makefile做的笔记](https://www.bilibili.com/video/BV1Bv4y1J7QT/?spm_id_from=333.337.top_right_bar_window_custom_collection.content.click&vd_source=fcf02d9420f87f9d13e80731eaba983f)，这篇文章使用的代码测试也是使用的他视频提供的代码。但是我只实现了Makefile基础部分的内容，后面的内容没有实现。

## Makefile基础

我们可以先查看一下我们树莓派的make版本，下面是我的打印信息，有版本，许可等。

```bash
(myenv) pi@raspberrypi:~/Example/12_Makefile $ make --version
GNU Make 4.3
为 aarch64-unknown-linux-gnu 编译
Copyright (C) 1988-2020 Free Software Foundation, Inc.
许可证：GPLv3+：GNU 通用公共许可证第 3 版或更新版本<http://gnu.org/licenses/gpl.html>。
本软件是自由软件：您可以自由修改和重新发布它。
在法律允许的范围内没有其他保证。
```

建立一个Makefile文件，我们先写一个简易的文件，Makefile就是一个编译和链接多个文件的工具，下面这个就是一个简单的例子，hello是目标文件，hello.cpp是依赖，再另起一行的就是怎么使用这个依赖生成目标。

```makefile
hello: hello.cpp
	g++ hello.cpp -o hello # 开头必须为一个Tab，不能为空格
```

写一个复杂一点的，也就是他提供的这个项目的Makefile，在复制的时候注意他的另起一行要用Tab，不能是空格，不然会报错**Makefile:2: 缺失分隔符。 停止。**

```makefile
sudoku: block.o command.o input.o main.o scene.o test.o
	g++ -o sudoku block.o command.o input.o main.o scene.o test.o

block.o: block.cpp common.h block.h color.h
	g++ -c block.cpp

command.o: command.cpp scene.h common.h block.h command.h
	g++ -c command.cpp

input.o: input.cpp common.h utility.inl
	g++ -c input.cpp

main.o: main.cpp scene.h common.h block.h command.h input.h
	g++ -c main.cpp

scene.o: scene.cpp common.h scene.h block.h command.h utility.inl
	g++ -c scene.cpp

test.o: test.cpp test.h scene.h common.h block.h command.h
	g++ -c test.cpp

hello.o: hello.cpp
	g++ -c hello.cpp


clean:
	rm block.o command.o input.o main.o scene.o test.o
	rm sudoku
```

### 最终目标

1. Makefile中会有很多目标，但最终目标只有一个，其他所有内容都是为这个最终目标服务的，写Makefile的时候**先写出最终目标，再依次解决总目标的依赖**

2. 一般情况第一条规则中的目标会被确立为最终目标，第一条规则默认会被make执行。

3. 通常来说目标是一个文件，一条规则的目的就是生成或更新目标文件。

4. make会根据目标文件和依赖文件最后修改时间判断是否需要执行更新目标文件的方法。如果目标文件不存在或者目标文件最后修改时间早于其中一个依赖文件最后修改时间，则重新执行更新目标文件的方法。否则不会执行。

5. 除了最终目标对应的更新方法默认会执行外，如果Makefile中一个目标不是其他目标的依赖，那么这个目标对应的规则不会自动执行。需要手动指定，方法为

```makefile
make <target>  # 如 make clean , make hello.o
```

6. 可以使用.DEFAULT_GOAL来修改默认最终目标

```makefile
.DEFAULT_GOAL = main

all: 
    @echo all

main:
    @echo main
```

### 伪目标

如果我们文件目录下有一个文件和我们的执行目标重名，就会冲突，比如我们的Makefile文件中有clean，如果文件夹中还有clean就会冲突。这时候我们一般会给这个目标加一个标识`.PHONY`，设置为伪目标，就不会冲突了。

```makefile
.PHONY: clean
clean:
	rm block.o command.o input.o main.o scene.o test.o
	rm sudoku.exe
```

### 依赖

依赖分为两种，一种是普通依赖，就是我们上面介绍的那种，一种是**order-only依赖**，但依赖文件更新并不会导致目标文件的更新。比如下面的用|分割的`test.o`就是这种依赖，他的更新不会引起sudoku的更新。

```makefile
sudoku: block.o command.o input.o main.o scene.o | test.o
```

### 变量

变量可以用来就是代替重复出现的内容。

如上面的makefile文件

```makefile
objects = block.o command.o input.o main.o scene.o test.o
sudoku: $(objects)
	g++ -o sudoku $(objects)

#省略中间......

clean:
	rm $(objects)
	rm sudoku
```

变量，只有字符串，有几种常用变量的方式，`=`是延迟展开，既Makefile文件已经构建好了，想要使用的时候才展开。`:=`是立即展开，就类似一行行执行。

```c
files = main.cpp hello.cpp
objects := main.o hello.o
```

变量还有几种常用的方法

#### 条件赋值

他会检查前面是否有定义，有定义就不执行

```makefile
var1 = 100
var1 ?= 200
```

#### 追加赋值

他会在原变量后面添加新的值

```makefile
files = main.cpp
files += hello.cpp
```

#### shell运行赋值

他会把shell指令执行的结果赋值给变量

```makefile
gcc_version != gcc --version
files != ls .
```

#### 多行变量

常见的变量都是单行的，多行变量常用于定义shell指令

```makefile
define <varable_name>  # 默认为 = 
# 变量内容
endef

define <varable_name> :=
# 变量内容
endef

define <varable_name> +=
# 变量内容
endef
```

#### 清楚变量

如果想清除一个变量，用以下方法

```makefile
undefine <变量名>   如 undefine files,  undefine objs
```

#### 环境变量的使用

系统中的环境变量可以直接在Makefile中直接使用，使用方法跟普通变量一样

```makefile
all:
    @echo $(USERNAME)
    @echo $(JAVA_HOME)
    @echo $(SystemRoot)
```

#### 变量替换引用

语法：__\$(var:a=b)__，意思是将变量var的值当中每一项结尾的a替换为b，直接上例子

```makefile
files = main.cpp hello.cpp
objs := $(files:.cpp=.o) # main.o hello.o
# 另一种写法
objs := $(files:%.cpp=%.o)
```

#### 变量覆盖

所有在Makefile中的变量，都可以在执行make时能过指定参数的方式进行覆盖。

```makefile
OverridDemo := ThisIsInMakefile
all:
    @echo $(OverridDemo)
```

如果直接执行

```shell
make
```

则上面的输出内容为*ThisIsInMakefile*，但可以在执行make时指定参数：

```shell
make OverridDemo=ThisIsFromOutShell # 等号两边不能有空格
# 如果变量值中有空格，需要用引号
make OverridDemo=“This Is From Out Shell”
```

则输出OverridDemo的值是ThisIsFromOutShell或This Is From Out Shell。

用这样的命令参数会覆盖Makefile中对应变量的值，如果不想被覆盖，可以在变量前加上override指令，override具有较高优先级，不会被命令参数覆盖

```makefile
override OverridDemo := ThisIsInMakefile
all:
    @echo $(OverridDemo)
```

这样即使命令行指定参数

```shell
make OverridDemo=ThisIsFromOutShell
```

输出结果依然是ThisIsInMakefile

### 自动变量

上面的Makefile文件，看着还是很长，makefile中有许多自动变量可以批量替换文件

```makefile
$@：①本条规则的目标名；②如果目标是归档文件的成员，则为归档文件名；③在多目标的模式规则中, 为导致本条规则方法执行的那个目标名；
$<：本条规则的第一个依赖名称
$?：依赖中修改时间晚于目标文件修改时间的所有文件名，以空格隔开
$^：所有依赖文件名，文件名不会重复，不包含order-only依赖
$+：类似上一个， 表示所有依赖文件名，包括重复的文件名，不包含order-only依赖
$|：所有order-only依赖文件名
$*：(简单理解)目标文件名的主干部分(即不包括后缀名)
$%：如果目标不是归档文件，则为空；如果目标是归档文件成员，则为对应的成员文件名
```

以下变量对应上述变量，D为对应变量所在的目录，结尾不带/，F为对应变量除去目录部分的文件名

__\$(@D)__  、__\$(@F)__、__\$(*D)__、__\$(*F)__、__\$(%D)__、__\$(%F)__、__\$(<D)__、__\$(<F)__、__\$(^D)__、__\$(^F)__、__\$(+D)__、__\$(+F)__、__\$(?D)__、__\$(?F)__

按照这个自动变量的解释，我们把上面的makefile进行改进

```makefile
sudoku: $(objects)
	g++ -o $@ $(objects)

%.o: %.cpp  
	g++ -c $<

clean:
	rm $(objects)
	rm sudoku 
```

其实这个就是一个简易的比较完整的Makefile了我感觉。

## 条件判断

使用条件指令可以让make执行或略过Makefile文件中的一些部分。

**ifdef**  判断一个变量是已否定义

```makefile
ifdef Win
    OS = Windows
else ifdef Mac
    OS= MacOS
else 
    OS = Linux
endif
```

**ifndef** 判断一个变量是否没被定义

```makefile
ifndef FLAGS
    FLAGS = -finput-charset=utf-8
endif
```

**ifeq** 判断两个值是否相等

```makefile
version = 3.0

ifeq ($(version),1.0)            # ifeq后一定要一个空格
    msg := 版本太旧了，请更新版本
else ifeq ($(version), 3.0)
    msg := 版本太新了，也不行
else
    msg := 版本可以用
endif

# 另外的写法
msg = Other
ifeq "$(OS)" "Windows_NT"
    msg = This is a Windows Platform
endif
```

**ifneq** 判断两个值是否不等

用法及参数同ifeq，只是判断结果相反

## 处理函数

从这里开始，我感觉就和上面的文件没有太大关系了，很多都是一些函数的用法。

### 字符替换与分析

#### **subst**

文本替换函数，返回替换后的文本，下面这个例子，就会将objs中的.o替换成.cpp，他是不管后缀的，是直接文本替换。

```makefile
objs = main.o hello.o
srcs = $(subst .o,.cpp,$(objs))
```

**patsubst**

模式替换， 返回替换后的文本，这个会把.o结尾的，替换成.cpp，比如这里第一个.o和hello中间没有空格，他就不会被替换。

```makefile
objs = main.ohello.o
srcs = $(subst %.o,%.cpp,$(objs)) 
```

#### **strip**

去除字符串头部和尾部的空格，中间如果连续有多个空格，则用一个空格替换，返回去除空格后的文本

```makefile
files = aa hello.cpp      main.cpp     test.cpp
files2 = $(strip $(files))
```

#### findstring

查找字符串，如果找到了，则返回对应的字符串，如果没找到，则反回空串

```makefile
files = hello.cpp main.cpp test.cpp
find = $(findstring hel,$(files))
find = $(findstring HEL,$(files))
```

#### filter

从文本中筛选出符合模式的内容并返回

```makefile
files = hello.cpp main.cpp test.cpp main.o hello.o hello.h
files2 = $(filter %.o %.h,$(files))
```

#### filter-out

与filter相反，过滤掉符合模式的，返回剩下的内容

```makefile
files = hello.cpp main.cpp test.cpp main.o hello.o hello.h
files2 = $(filter-out %.o %.cpp,$(files))
```

#### sort

将文本内的各项按字典顺序排列，并且移除重复项

```makefile
files = hello.cpp main.cpp test.cpp main.o hello.o hello.h main.cpp hello.cpp
files2 = $(sort $(files))
```

#### word

用于返回文本中第n个单词

```makefile
files = hello.cpp main.cpp test.cpp main.o hello.o hello.h main.cpp hello.cpp
files2 = $(word 3,$(files))
```

#### wordlist

用于返回文本指定范围内的单词列表

```makefile
files = hello.cpp main.cpp test.cpp main.o hello.o hello.h main.cpp hello.cpp
files2 = $(wordlist 3,6,$(files))
```

#### words

返回文本中单词数

```makefile
files = hello.cpp main.cpp test.cpp main.o hello.o hello.h main.cpp hello.cpp
nums = $(words $(files))
```

#### firstword

返回第一个单词

```makefile
$(firstword text)
```

#### lastword

返回最后一个单词

```makefile
$(lastword text)
```

### 文件名处理函数

#### dir

返回文件目录

```makefile
$(dir files)
        --- files 需要返回目录的文件名，可以有多个，用空格隔开

files = src/hello.cpp main.cpp

files2 = $(dir $(files))
```

#### notdir

返回除目录部分的文件名

```makefile
$(notdir files)
        --- files 需要返回文件列表，可以有多个，用空格隔开

files = src/hello.cpp main.cpp
files2 = $(notdir $(files))
```

#### suffix

返回文件后缀名，如果没有后缀返回空

```makefile
$(suffix files)
        --- files 需要返回后缀的文件名，可以有多个，用空格隔开


files = src/hello.cpp main.cpp hello.o hello.hpp hello
files2 = $(suffix $(files))
```

#### basename

返回文件名除后缀的部分

```makefile
files = src/hello.cpp main.cpp hello.o hello.hpp hello
files2 = $(basename $(files))
```

#### addsuffix

给文件名添加后缀

```makefile
files = src/hello.cpp main.cpp hello.o hello.hpp hello
files2 = $(addsuffix .exe,$(files))
```

#### addprefix

给文件名添加前缀

```makefile
files = src/hello.cpp main.cpp hello.o hello.hpp hello
files2 = $(addprefix make/,$(files))
```

#### join

将两个列表中的内容一对一连接，如果两个列表内容数量不相等，则多出来的部分原样返回

```makefile
f1 = hello main test
f2 = .cpp .hpp
files2 = $(join $(f1),$(f2))
```

#### wildcard

返回符合通配符的文件列表

```makefile
files2 = $(wildcard *.cpp)
files2 = $(wildcard *)
files2 = $(wildcard src/*.cpp)
```

#### realpath

返回文件的绝对路径

```makefile
f3 = $(wildcard src/*)
files2 = $(realpath $(f3))
```

#### abspath

返回绝对路径，用法同realpath，如果一个文件名不存在，realpath不会返回内容，abspath则会返回一个当前文件夹一下的绝对路径

```makefile
$(abspath files)
```

### 条件函数

#### if

条件判断，如果条件展开不是空串，则反回真的部分，否则返回假的部分

```makefile
files = src/hello.cpp main.cpp hello.o hello.hpp hello
files2 = $(if $(files),有文件,没有文件)
```

#### or

返回条件中第一个不为空的部分

```makefile
f1 = 
f2 = 
f3 = hello.cpp
f4 = main.cpp
files2 = $(or $(f1),$(f2),$(f3),$(f4))
```

#### and

如果条件中有一个为空串，则返回空，如果全都不为空，则返回最后一个条件

```makefile
f1 = 12
f2 = 34
f3 = hello.cpp
f4 = main.cpp
files2 = $(and $(f1),$(f2),$(f3),$(f4))
```

#### intcmp

比较两个整数大小，并返回对应操作结果（GNU make 4.4以上版本）

```makefile
$(intcmp lhs,rhs[,lt-part[,eq-part[,gt-part]]]) 
        --- lhs 第一个数
        --- rhs 第二个数
        --- lt-part  lhs < rhs时执行
        --- eq-part  lhs = rhs时执行
        --- gt-part  lhs > rhs时执行
        --- 如果只提供前两个参数，则lhs == rhs时返回数值，否则返回空串
            参数为lhs,rhs,lt-part时，当lhs < rhs时返回lt-part结果，否则返回空
            参数为lhs,rhs,lt-part,eq-part，lhs < rhs返回lt-part结果，否则都返回eq-part结果
            参数全时，lhs < rhs返回lt-part，lhs == rhs返回eq-part, lhs > rhs返回gt-part



@echo $(intcmp 2,2,-1,0,1)
```

### file

读写文件

```makefile
$(file op filename[,text])
        --- op 操作
                > 覆盖
                >> 追加
                < 读
        --- filename 需要操作的文件名
        --- text 写入的文本内容，读取是不需要这个参数


files = src/hello.cpp main.cpp hello.o hello.hpp hello
write = $(file > makewrite.txt,$(files))
read = $(file < makewrite.txt)
```

### foreach

对一列用空格隔开的字符序列中每一项进行处理，并返回处理后的列表

```makefile
$(foreach each,list,process)
        --- each list中的每一项
        --- list 需要处理的字符串序列，用空格隔开
        --- process 需要对每一项进行的处理

list = 1 2 3 4 5
result = $(foreach each,$(list),$(addprefix cpp,$(addsuffix .cpp,$(each))))
```

作用类似C/C++中的循环

```c++
int list[5] = {1, 2, 3, 4, 5};
int result[5];
int each;
for(int i = 0; i < 5; i++)
{
    each = list[i];
    result[i] = process(each);
}
// 此时result即为返回结果
```

### call

将一些复杂的表达式写成一个变量，用call可以像调用函数一样进行调用。类似于编程语言中的自定义函数。在函数中可以用$(n)来访问第n个参数

```makefile
$(call funcname,param1,param2,…)
        --- funcname 自定义函数（变量名）
        --- 参数至少一个，可以有多个，用逗号隔开

dirof =  $(dir $(realpath $(1))) $(dir $(realpath $(2)))
result = $(call dirof,main.cpp,src/hello.cpp)
```

### value

对于不是立即展开的变量，可以查看变量的原始定义；对于立即展开的变量，直接返回变量值

```makefile
$(value variable)

var = value function test
var2 = $(var)
var3 := $(var)
all:
    @echo $(value var2)
    @echo $(value var3)
```

### origin

查看一个变量定义来源

```makefile
$(origin variable)


var2 = origin function 
all:
    @echo $(origin var1)    # undefined 未定义
    @echo $(origin CC)        # default 默认变量
    @echo $(origin JAVA_HOME) # environment 环境变量
    @echo $(origin var2)    # file 在Makefile文件中定义的变量
    @echo $(origin @)        # automatic 自动变量
```

### flavor

查看一个变量的赋值方式

```makefile
$(flavor variable)

var2 = flavor function
var3 := flavor funciton
all:
    @echo $(flavor var1)    # undefined 未定义
    @echo $(flavor var2)    # recursive 递归展开赋值
    @echo $(flavor var3)    # simple 简单赋值
```

### eval

可以将一段文本生成Makefile的内容

```makefile
$(eval text)

define eval_target = 
eval:
    @echo Target Eval Test
endef

$(eval $(eval_target))
```

以上，运行make时将会执行eval目标

### shell

用于执行Shell命令

```makefile
files = $(shell ls *.cpp)
$(shell echo This is from shell function)
```

### let

将一个字符串序列中的项拆开放入多个变量中，并对各个变量进行操作（GNU make 4.4以上版本）

```makefile
$(let var1 [var2 ...],[list],proc)
        --- var 变量，可以有多个，用空格隔开
        --- list 待处理字符串，各项之间空格隔开
        --- proc 对变量进行的操作，结果为let的返回值
            将list中的值依次一项一项放到var中，如果var的个数多于list项数，那多出来的var是空串。如果
            var的个数小于list项数，则先依次把前而的项放入var中，剩下的list所有项都放入最后一个var中


list = a b c d
letfirst = $(let first second rest,$(list),$(first))
letrest = $(let first second rest,$(list),$(rest))


# 结合call可以对所有项进行递归处理
reverse = $(let first rest,$(1),$(if $(rest),$(call reverse,$(rest)) )$(first))
all: ; @echo $(call reverse,d c b a)
```

### 信息提示函数

#### error

提示错误信息并终止make执行

```makefile
$(error text)
        --- text 提示信息

EXIT_STATUS = -1
ifneq (0, $(EXIT_STATUS))
    $(error An error occured! make stopped!)
endif
```

#### warning

提示警告信息，make不会终止

```makefile
$(warning text)

ifneq (0, $(EXIT_STATUS))
    $(warning This is a warning message)
endif
```

#### info

输出一些信息

```makefile
$(info text…)

$(info 编译开始.......)
$(info 编译结束)
```

# 
