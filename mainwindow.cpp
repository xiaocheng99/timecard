#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QBuffer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("考勤打卡系统");
    tcpClient = new QTcpSocket;
    connect(timer,&QTimer::timeout,this,&MainWindow:: readCapture);
    connect(tcpClient,&QTcpSocket::connected,this,&MainWindow::onconnected);
    connect(tcpClient,&QTcpSocket::readyRead,this,&MainWindow::on_ready_read);
}
void MainWindow::onconnected()
{
    qDebug()<<"connected!";
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_open_clicked()
{
    capture.open(0);
    timer->start(33);
}

void MainWindow::disply_emp_info(QString name){
    ui->label_name->setText(name);
    ui->label_state->setText("successed");
}

void MainWindow::on_ready_read()
{

    QByteArray response_data = tcpClient->readAll();
    qDebug() << "第一次打印信息：";
    qDebug() << response_data.toStdString().data();

    //处理json
    QJsonParseError jsonError;

    QJsonDocument document = QJsonDocument::fromJson(response_data,&jsonError);
        if(jsonError.error != QJsonParseError::NoError){

            qDebug() << QStringLiteral("resolve Json failed");
        }

        if(document.isObject()){
                QJsonObject obj = document.object();
                if(obj.contains("way")){
                    QJsonValue result_code = obj.take("way");
                    if (result_code.toString()=="recognizeResult")
                    {
                        if(obj.contains("name")){
                            QJsonValue result_name = obj.take("name");
                            qDebug() << "第三次打印信息：";
                            qDebug() << result_name.toString();
                            disply_emp_info(result_name.toString());
                        }
                    }

                    }

                }


}
void MainWindow::readCapture()
{
    Mat frame;
   capture>>frame;
   image =cvMat2QImage(frame);
    pixmap = QPixmap::fromImage(image);
   ui->label_video->setPixmap(pixmap);
}


QImage MainWindow::cvMat2QImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
        if(mat.type() == CV_8UC1)
        {
            QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
            // Set the color table (used to translate colour indexes to qRgb values)
            image.setColorCount(256);
            for(int i = 0; i < 256; i++)
            {
                image.setColor(i, qRgb(i, i, i));
            }
            // Copy input Mat
            uchar *pSrc = mat.data;
            for(int row = 0; row < mat.rows; row ++)
            {
                uchar *pDest = image.scanLine(row);
                memcpy(pDest, pSrc, mat.cols);
                pSrc += mat.step;
            }
            return image;
        }
        // 8-bits unsigned, NO. OF CHANNELS = 3
        else if(mat.type() == CV_8UC3)
        {
            // Copy input Mat
            const uchar *pSrc = (const uchar*)mat.data;
            // Create QImage with same dimensions as input Mat
            QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
            return image.rgbSwapped();
        }
        else if(mat.type() == CV_8UC4)
        {
            qDebug() << "CV_8UC4";
            // Copy input Mat
            const uchar *pSrc = (const uchar*)mat.data;
            // Create QImage with same dimensions as input Mat
            QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
            return image.copy();
        }
        else
        {
            qDebug() << "ERROR: Mat could not be converted to QImage.";
            return QImage();
        }


}



void MainWindow::on_btn_capture_clicked()
{
    Mat frame;
    capture>>frame;
    QImage image =cvMat2QImage(frame);
    QPixmap pixmap = QPixmap::fromImage(image);
   //image.save("D:\\Desktop\\FILE\\picture\\timecard\\lcc.jpg");
    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
   //pixmap不能为空，必须先将图片加载到pixmap中
       pixmap.save(&buffer,"jpg");
       QByteArray pixArray;
       pixArray.append(buffer.data());
       QByteArray pixArry64 = pixArray.toBase64();
       //创建json对象并插入数据
       QJsonObject json;
       json.insert("way","recognize");
       json.insert("data",QJsonValue::fromVariant(pixArry64));
       //把json对象转为文件二进制
       QJsonDocument document;
       document.setObject(json);
       QByteArray jsonArry = document.toJson(QJsonDocument::Compact);
       //把json数据发送给服务器
       tcpClient->write(jsonArry);

}

void MainWindow::on_btn_close_clicked()
{
    capture.release();
    timer->stop();
    ui->label_video->clear();
}

void MainWindow::on_btn_connect_clicked()
{
    QString addr = ui->lineEdit_IP->text();
    quint16 port = ui->lineEdit_post->text().toUInt();
    tcpClient->connectToHost(addr,port);

}
