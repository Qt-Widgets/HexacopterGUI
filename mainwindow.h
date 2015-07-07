#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "glwidget.h"

#include "worldmagneticmodel.h"

#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include "gateway-qt/api/gateway.h"
#include "gateway-qt/api/hal.h"

#include "opticaltracking.h"
#include "datatypes.h"

#include "interfacedialog.h"
#include "cameradialog.h"

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include "Mapplot/mapplot.h"

#include "screen2.h"

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
    void initGateway(bool serial, QStringList data);
    void stopGateway(bool serial);
    void openDev(int i);
    void closeDev();
    void resendMsg();

signals:
    void imuChanged(float roll,float pitch,float yaw);
    void gpsLLH(double lat, double lon, double height);
    void gpsNED(double east, double north);
    void hmdHeight(float height);
    void setHeading(float angle);
    void setShipDirection(double x, double y);
    void stopFlightTime();
    void startFlightTime();

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
    void on_pushButton_save_alt_clicked();
    void on_pushButton_load_alt_clicked();
    void on_setOPID_alt_clicked();
    void on_actionConnect_triggered(bool checked);
    void processFrame();

    void on_actionCamera_triggered();

    void on_checkBox_screen_clicked(bool checked);

    void on_pushButton_resetGPS_clicked();
    void on_setOPID_pos_clicked();
    void on_pushButton_save_pos_clicked();
    void on_pushButton_load_pos_clicked();
    void on_set_Goal_Pose_clicked();
    void on_frameSelect_currentIndexChanged(int index);

protected:
    void keyPressEvent(QKeyEvent* event);

private:

    void parseTopicData(const long topicId, const unsigned int len, const void* msg, const NetMsgInfo& netMsgInfo);
    void parseTopicLimitedData(const long topicId, const unsigned int len, const void* msg, const NetMsgInfo& netMsgInfo);

    HAL_UART_UDP *uart_udp;
    LinkinterfaceUART_UDP *linkif_udp;
    HAL_UART *uart;
    LinkinterfaceUART *linkif;

    Gateway *gw;

    cv::VideoCapture capVideo;
    cv::Mat frame;
    QTimer* tmrCam;

    WorldMagneticModel wmm;
    double bearing;

    InterfaceDialog diag;
    cameradialog camdiag;
    void parse(QByteArray data);
    QTimer *polltmr;
    QTimer *timeoutTmr;
    Ui::MainWindow *ui;
    GLWidget *glWidget;

    Screen2 scHMD;
    bool screenActive;

    QByteArray message;
    bool ff;
    bool gettingMessage;
    double start, lastTime;
    int cntSelectedTopics;
    OpticalTracking *ot;
    PID_values_t rpy[3];
    int lastIndex;

    QGridLayout *videoLayoutBig, *smallLayout, *mapLayoutBig;
    QLabel *videoView;

    Mapplot *map;
};

#endif // MAINWINDOW_H
