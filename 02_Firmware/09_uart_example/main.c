#include <stdio.h>
#include<wiringPi.h>
#include<wiringSerial.h>

int main(int argc,char **argv)
{
    int MySerial;

    if (wiringPiSetup()<0)
    {
        printf("Setup Failed!\n") ;
        return 1;
    }

    if ((MySerial=serialOpen("/dev/ttyS0", 115200))<0)
    {
        printf("Serial Failed!\n");
        return 1;
    }

    while (1)
    {
        if (serialDataAvail(MySerial)>0)
        {
            char ch=serialGetchar(MySerial);
            //serialPrintf(MySerial,"Hello World!\n"); 
            serialPutchar(MySerial,ch); 
        }
    }

    serialClose(MySerial);

    return 0;
}