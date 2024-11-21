import smbus                 #在程序中导入“smbus”模块（系统管理总线模块）
import time
 
# 0 代表 /dev/i2c-0， 1 代表 /dev/i2c-1 ,具体看使用的树莓派那个I2C来决定
bus = smbus.SMBus(1)         #创建一个smbus实例
 
#在树莓派上查询PCF8591的地址：“sudo i2cdetect -y 1”
def setup(Addr):
    global address
    address = Addr
 
def read(chn): #channel
    if chn == 0:
        bus.write_byte(address,0x40)   #发送一个控制字节到设备
    if chn == 1:
        bus.write_byte(address,0x41)
    if chn == 2:
        bus.write_byte(address,0x42)
    if chn == 3:
        bus.write_byte(address,0x43)
    bus.read_byte(address)             #从设备读取单个字节，而不指定设备寄存器。
    return bus.read_byte(address)      #返回某通道输入的模拟值A/D转换后的数字值
 
def write(val):
    temp = val  # 将字符串值移动到temp
    temp = int(temp) # 将字符串改为整数类型
    # print temp to see on terminal else comment out
    bus.write_byte_data(address, 0x40, temp) 
    #写入字节数据，将数字值转化成模拟值从AOUT输出
 
if __name__ == "__main__":
    setup(0x48) 
 #在树莓派终端上使用命令“sudo i2cdetect -y 1”，查询出PCF8591的地址为0x48
    while True:
        print ('电位计   AIN0 = ', read(0))   #电位计模拟信号转化的数字值
        print ('光敏电阻 AIN1 = ', read(1))  #光敏电阻模拟信号转化的数字
        print ('热敏电阻 AIN2 = ', read(2))   #热敏电阻模拟信号转化的数字值
        tmp = read(0)
        tmp = tmp*(255-125)/255+125 
# 125以下LED不会亮，所以将“0-255”转换为“125-255”，调节亮度时灯不会熄灭
        write(tmp)
        time.sleep(2)