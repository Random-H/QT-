#ifndef MYTHREA_H
#define MYTHREA_H

#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMutex>
#include "ui_widget.h"

class MyThread : public QThread
{
    Q_OBJECT
public:
    Ui::Widget *myui;
    explicit MyThread(QObject *parent = nullptr);
    void run() ;
    void threadpause();//线程暂停
    void threadresume();//线程重启
    void setflag(bool status);
signals:
private slots:

private:
    bool flag = true;
    QSerialPort *serialport;
    QList<QSerialPortInfo>AllSerialport;
    int CountOfSerial = 0;
    QMutex t_lock;
};

#endif // MYTHREA_H
