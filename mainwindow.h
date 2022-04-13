#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "opencv2/video.hpp"
#include "opencv/cv.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QTimer>
#include <QTcpSocket>
#include <qtcpserver.h>

using namespace  cv;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void readCapture();
    QTcpSocket *tcpClient;
    QImage cvMat2QImage(const cv::Mat& mat);
    VideoCapture capture;
    QImage image;
    QPixmap pixmap;
    QTimer *timer = new QTimer();
    
    void disply_emp_info(QString name);

private slots:
    void on_btn_open_clicked();

    void on_btn_capture_clicked();

    void on_btn_close_clicked();

    void on_btn_connect_clicked();

    void onconnected();

    void on_ready_read();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
