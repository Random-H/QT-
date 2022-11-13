#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{   
    ui->setupUi(this);

    WindowInit();
    resize(1200,900);
    setWindowTitle("串口助手");
    setWindowIcon(QIcon(":/ico/tb.ico"));
    mythread = new MyThread(this);
    mythread->myui = ui;
    serialport = new QSerialPort(this);
    connect(serialport,&QSerialPort::readyRead,this,&Widget::DealReceviceData);//接收数据
    connect(this,&Widget::destroyed,this,&Widget::SaveConf);
    connect(this,&Widget::destroyed,this,&Widget::all_close);
    mythread->start();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::all_close()
{
    mythread->threadresume();
    mythread->setflag(false);

    mythread->quit();
    mythread->wait();
    serialport->close();
}

void Widget::SaveConf()
{
    /*保存上一次使用时的配置，波特率，数据位....*/
    QString FilePath = "SerialPortAssistantConf.txt";
    if(FilePath.isEmpty() == false)
    {
        QFile file;
        file.setFileName(FilePath);
        bool isOK = file.open(QIODevice::WriteOnly);
        if(isOK == true)
        {
            QString data = "BaudRate:"
                    +ui->BaudRate->currentText()+"\r\n"
                    +"DataBits:"
                    +ui->DataBit->currentText()+"\r\n"
                    +"Parity:"
                    +ui->ParityBit->currentText()+"\r\n"
                    +"StopBits:"
                    +ui->StopBit->currentText()+"\r\n";
            file.write(data.toUtf8());
        }
        file.close();
    }
}

void Widget::DealReceviceData()
{
    current_time =QTime::currentTime();
    minute = current_time.minute(); //分
    second = current_time.second(); //秒
    hour = current_time.hour();//时

    GetSendOrReceviceType();

    QByteArray Rececive_data_byte;
    QString Rececive_data;

    if(SerialPortIsOpen == true)
    {
        Rececive_data_byte = serialport->readAll();
        RBC += Rececive_data_byte.count();//统计接收数
        ui->ReceviceByteCount->setText(QString::number(RBC));

        if(HexType == true)
        {
            QString strDis;
            QString str3 = Rececive_data_byte.toHex().data();//以十六进制显示
            str3 = str3.toUpper ();//转换为大写
            for(int i = 0;i<str3.length ();i+=2)//填加空格
            {
                QString st = str3.mid (i,2);
                strDis += st;
                strDis += " ";
            }
            ui->Read_TextEdit->append("R:["+QString::number(hour)+":"
                                      +QString::number(minute)+":"
                                      +QString::number(second)+"] "
                                      +strDis);
        }
        else
        {
            /***************转换编码***************/
            textcodec = QTextCodec::codecForName("GBK");
            Rececive_data = textcodec->toUnicode(Rececive_data_byte);

            ui->Read_TextEdit->append("R:["+QString::number(hour)+":"
                                      +QString::number(minute)+":"
                                      +QString::number(second)+"] "
                                      +Rececive_data);
        }
    }
}

void Widget::SetSerial()
{
    /*配置端口*/
    serialport->setPortName(ui->SerialPortNameBox->currentText());
    serialport->setBaudRate((ui->BaudRate->currentText()).toInt());//波特率
    QString Setparity = ui->ParityBit->currentText();
    QString Setdatbits = ui->DataBit->currentText();
    QString Setstopbits = ui->StopBit->currentText();
    if(Setparity == "NoParity")
        serialport->setParity(QSerialPort::NoParity);//无校验
    else if (Setparity == "EvenParity")
        serialport->setParity(QSerialPort::EvenParity);//无校验
    else if (Setparity == "OddParity")
        serialport->setParity(QSerialPort::OddParity);//无校验
    else if (Setparity == "SpaceParity")
        serialport->setParity(QSerialPort::SpaceParity);//无校验
    else if (Setparity == "MarkParity")
        serialport->setParity(QSerialPort::MarkParity);//无校验


    if(Setdatbits == "5")
        serialport->setDataBits(QSerialPort::Data5);//数据位
    else if(Setdatbits == "6")
        serialport->setDataBits(QSerialPort::Data6);//数据位
    else if(Setdatbits == "7")
        serialport->setDataBits(QSerialPort::Data7);//数据位
    else if(Setdatbits == "8")
        serialport->setDataBits(QSerialPort::Data8);//数据位

    if(Setstopbits == "0")
        serialport->setStopBits(QSerialPort::OneStop);//停止位
    else if(Setstopbits == "1.5")
        serialport->setStopBits(QSerialPort::OneAndHalfStop);//停止位
    else if(Setstopbits == "2")
        serialport->setStopBits(QSerialPort::TwoStop);//停止位
}

void Widget::on_OpenSerialPort_clicked()
{
    if(SerialPortIsOpen == true)
    {
        SerialPortIsOpen = false;
        serialport->close();
        ui->OpenSerialPort->setText("打开串口");
        mythread->threadresume();
        return;
    }
    SetSerial();//设置端口
    serialport->open(QSerialPort::ReadWrite);
    if(serialport->isOpen())
    {
        SerialPortIsOpen = true;
        qDebug()<<"串口打开成功";
        ui->OpenSerialPort->setText("关闭串口");
        mythread->threadpause();
    }
    else
    {
        SerialPortIsOpen = false;
        qDebug()<<"串口打开失败";
        QMessageBox::critical(this,"错误","端口可能已被占用");

    }
}

void Widget::on_Send_clicked()
{
    GetSendOrReceviceType();
    QString Send_Data = ui->Send_TextEdit->toPlainText();//.toStdString().c_str();

    SBC += Send_Data.count();//统计发送
    ui->SendByteCount->setText(QString::number(SBC));

    QByteArray data_f;
    textcodec = QTextCodec::codecForName("GBK");//转换编码

    if(SerialPortIsOpen == true)
    {
        if(HexType == true)
        {
            QByteArray data = Send_Data.toUtf8();
            data_f = QByteArray::fromHex(data);
            serialport->write(data_f);
        }
        else
        {
            serialport->write(textcodec->fromUnicode(Send_Data));
        }

        //ui->Send_TextEdit->clear();
        if(ui->ShowSendData->isChecked() == true)
        {
            current_time =QTime::currentTime();
            minute = current_time.minute(); //分
            second = current_time.second(); //秒
            hour = current_time.hour();//时
            if(HexType == true)
            {
                QString strDis;
                QString str3 = Send_Data.toUtf8().toHex().data();//以十六进制显示
                str3 = str3.toUpper ();//转换为大写
                for(int i = 0;i<str3.length ();i+=2)//填加空格
                {
                    QString st = str3.mid (i,2);
                    strDis += st;
                    strDis += " ";
                }
                ui->Read_TextEdit->append("S:["+QString::number(hour)+":"
                                          +QString::number(minute)+":"
                                          +QString::number(second)+"] "
                                          +strDis);

            }
            else
            {
                ui->Read_TextEdit->append("S:["+QString::number(hour)+":"
                                          +QString::number(minute)+":"
                                          +QString::number(second)+"] "
                                          +Send_Data);
            }

        }
    }
}

void Widget::on_Clear_Recevice_clicked()
{
    ui->Read_TextEdit->clear();
}

void Widget::on_Clear_Send_clicked()
{
    ui->Send_TextEdit->clear();
}

void Widget::on_SaveReceviceDara_clicked()
{
    /*保存接收数据*/
    QString FilePath = QFileDialog::getSaveFileName(this,"save","../","TXT(*.txt)");
    if(FilePath.isEmpty() == false)
    {
        QFile file;
        file.setFileName(FilePath);
        bool isOK = file.open(QIODevice::WriteOnly);
        if(isOK == true)
        {
            QString data = ui->Read_TextEdit->toPlainText();
            file.write(data.toUtf8());
        }
        file.close();
    }
}

void Widget::GetSendOrReceviceType()
{
    if(ui->HEX->isChecked() == true)//获取发送和接收为十六进制还是文本
    {
        HexType = true;
    }
    else if(ui->TXT->isChecked() == true)
    {
        HexType = false;
    }
}

void Widget::WindowInit()
{
    QString FilePath = "SerialPortAssistantConf.txt";
    if(FilePath.isEmpty() == false)
    {

        QFile file(FilePath);
        QString conf1,conf2,conf3;
        bool isOK = file.open(QIODevice::ReadOnly);//读取上一次配置

        if(isOK == true)
        {
            QByteArray array;
            while(file.atEnd() == false)
            {
                array = file.readLine();
                conf1 = array;
                QStringList conflist = conf1.split(":");
                conf3 = conflist[1];
                QStringList conflist2 = conf3.split("\r\n");
                conf2 = conflist2[0];
                qDebug()<<conflist[0];
                if(conflist[0] == "DataBits")
                {
                    ui->DataBit->setCurrentText(conf2);
                }
                else if(conflist[0] == "Parity")
                {
                    ui->ParityBit->setCurrentText(conf2);
                }
                else if(conflist[0] == "StopBits")
                {;
                    ui->StopBit->setCurrentText(conf2);
                }
                else if(conflist[0] == "BaudRate")
                {
                    ui->BaudRate->setCurrentText(conf2);
                }
            }
        }

        file.close();
    }
}

void Widget::on_ClearCount_clicked()
{
    /*清空计数*/
    SBC = 0;
    RBC = 0;
    ui->SendByteCount->setText(QString::number(SBC));
    ui->ReceviceByteCount->setText(QString::number(RBC));
}

void Widget::on_OpenFile_clicked()
{
    /*打开要发送的文件*/
    QString FilePath = QFileDialog::getOpenFileName(this,"Open","../","File(*.*)");
    if(FilePath.isEmpty() == false)
    {
        QFile file;
        file.setFileName(FilePath);

        bool isOK = file.open(QIODevice::ReadOnly);
        if(isOK == true)
        {
            openfile = file.readAll();
            FSC = openfile.count();
            ui->Send_TextEdit->append("FileName:"+FilePath+"\n"
                                       +"Size:"+QString::number(FSC)+"\n"
                                       +"Content："+openfile);
        }

        file.close();
    }
}

void Widget::on_SendFile_clicked()
{
    GetSendOrReceviceType();
    /*发送文件*/
    SBC += openfile.count();
    ui->SendByteCount->setText(QString::number(SBC));
    QByteArray data_f;
    textcodec = QTextCodec::codecForName("GBK");
    if(SerialPortIsOpen == true)
    {
        if(HexType == true)
        {
            data_f = QByteArray::fromHex(openfile);
            serialport->write(data_f);
        }
        else
        {
            serialport->write(textcodec->fromUnicode(openfile));
        }

        //ui->Send_TextEdit->clear();
        if(ui->ShowSendData->isChecked() == true)
        {
            current_time =QTime::currentTime();
            minute = current_time.minute(); //分
            second = current_time.second(); //秒
            hour = current_time.hour();//时
            if(HexType == true)
            {
                QString strDis;
                QString str3 = openfile.toHex().data();//以十六进制显示
                str3 = str3.toUpper ();//转换为大写
                for(int i = 0;i<str3.length ();i+=2)//填加空格
                {
                    QString st = str3.mid (i,2);
                    strDis += st;
                    strDis += " ";
                }
                ui->Read_TextEdit->append("S:["+QString::number(hour)+":"
                                          +QString::number(minute)+":"
                                          +QString::number(second)+"] "
                                          +strDis);

            }
            else
            {
                ui->Read_TextEdit->append("S:["+QString::number(hour)+":"
                                          +QString::number(minute)+":"
                                          +QString::number(second)+"] "
                                          +openfile);
            }

        }
    }
}
