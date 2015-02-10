#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "glwidget.h"
//#include <QtSerialPort/QSerialPort>

#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include "gateway-linux/api/gateway.h"
#include "gateway-linux/api/hal.h"

#include "opticaltracking.h"


static QString colorplate[] = {"aquamarine", "blue", "blueviolet", "brown", "cadetblue", "chartreuse",
                               "coral", "cornflowerblue", "crimson", "darkblue", "darkcyan",
                               "darkgoldenrod", "darkgreen", "darkmagenta", "darkolivegreen",
                               "darkorange", "darkorchid", "darkred", "darkslateblue",
                               "darkslategray", "darkturquoise", "darkviolet", "deeppink",
                               "deepskyblue", "dodgerblue", "firebrick", "forestgreen", "green"
                               "gold", "goldenrod", "greenyellow", "hotpink", "indigo",
                               "lawngreen", "lightseagreen", "limegreen", "mediumblue",
                               "mediumorchid", "mediumpurple", "mediumseagreen", "mediumslateblue",
                               "mediumspringgreen", "mediumturquoise", "mediumvioletred", "midnightblue",
                               "olivedrab", "orange", "orangered", "orchid", "red", "royalblue", "saddlebrown",
                               "seagreen", "springgreen", "steelblue", "tomato", "yellowgreen", "violet",
                               "turquoise"};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, Putter
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    virtual bool putGeneric(const long topicId, const unsigned int len, const void* msg, const NetMsgInfo& netMsgInfo);

public slots:
//    void receive();
    void pollGateway();

signals:
    void imuChanged(float roll,float pitch,float yaw);

private slots:
    void on_setOPID_clicked();
    void on_pushButton_start_clicked();
    void on_pushButton_stop_clicked();
    void on_pushButton_speed_clicked();
    void on_pushButton_load_clicked();
    void on_pushButton_save_clicked();
    void on_pushButton_selectAll_clicked();
    void on_pushButton_deselectAll_clicked();
    void on_pushButton_resetTime_clicked();
    void mousePress();
    void mouseWheel();
    void selectionChanged();
    void sendSelectedTopics();
    void get_OT_Attitude(double roll, double pitch, double yaw);

    void on_pushButton_saveSelected_clicked();
    void on_pushButton_plot_clicked();
    void on_pushButton_setRPY_clicked();

    void on_comboBox_currentIndexChanged(int index);

protected:
    void keyPressEvent(QKeyEvent* event);

private:
    void parse(QByteArray data);
    QTimer *timer;
    Ui::MainWindow *ui;
    GLWidget *glWidget;
//    QSerialPort *serial;
    QByteArray message;
    bool ff;
    bool gettingMessage;
    double start, lastTime;
    int cntSelectedTopics;
    OpticalTracking *ot;
};

#endif // MAINWINDOW_H
