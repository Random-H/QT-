#include "mythread.h"

MyThread::MyThread(QObject *parent) : QThread(parent)
{

}

void MyThread::run()
{
    /*串口关闭时循环检测电脑可用端口*/
    int yuanlai=0;
    while(flag)//如果串口打开则跳出循环
    {
        t_lock.lock();
        AllSerialport.clear();
        yuanlai = CountOfSerial;
        AllSerialport = QSerialPortInfo::availablePorts();
        CountOfSerial = AllSerialport.count();
        if(yuanlai<CountOfSerial || yuanlai>CountOfSerial)//如果有新增或减少可用端口
        {
            myui->SerialPortNameBox->clear();//端口combox清除
            for(int i = 0 ; i<CountOfSerial ; i++)//重新添加可用端口到combox
            {
                 myui->SerialPortNameBox->addItem(AllSerialport.at(i).portName());
            }
        }  
        if(flag == false)
        {
            break;
        }
        t_lock.unlock();
    }
}
void MyThread::threadpause()
{
   t_lock.lock();//不懂
}

void MyThread::threadresume()
{
   t_lock.unlock();
}

void MyThread::setflag(bool status)
{
    flag = status;//flag用于判断串口是否打开
}


