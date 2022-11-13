#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include <QTime>
#include <QTextCodec>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include "mythread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void SetSerial();
    void GetSendOrReceviceType();//十六进制或者文本格式
    void WindowInit();
    void all_close();//关闭
private slots:
    void DealReceviceData();
    void SaveConf();

    void on_Send_clicked();
    void on_Clear_Recevice_clicked();
    void on_Clear_Send_clicked();
    void on_SaveReceviceDara_clicked();
    void on_OpenFile_clicked();
    void on_OpenSerialPort_clicked();
    void on_ClearCount_clicked();
    void on_SendFile_clicked();
private:
    Ui::Widget *ui;
    MyThread *mythread;
    QSerialPort *serialport;
    int minute;
    int second;
    int hour;
    bool SerialPortIsOpen = false;
    bool HexType;
    QTime current_time;
    QTextCodec *textcodec;
    int SBC = 0;
    int RBC = 0;
    int FSC = 0;//文件大小;
    QByteArray openfile;
};
#endif // WIDGET_H
