#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "glwidget.h"

#include <QDebug>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QSerialPort>

#include "datatypes.h"
#include "matlib.h"
#include "topics.h"
#include "myitem.h"

#define MY_NORTH    3.0f //30.8f

//#define OT

static double yAxis = 50;
static double xAxis = 15;

int64_t NOW(){
    struct timeval t;
    gettimeofday(&t,0);
    return (t.tv_sec*1000000LL+t.tv_usec)*1000;
}



int cnt2=0;

uint32_t bigEndianToInt32_t(const void* buff) {
    uint8_t* byteStream = (uint8_t*)buff;
    if (byteStream == 0) return 0;
    return    (((uint32_t)(byteStream[0])) << 24)
            | (((uint32_t)(byteStream[1])) << 16)
            | (((uint32_t)(byteStream[2])) <<  8)
            |  ((uint32_t)(byteStream[3]));
}



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    Putter(),
    diag(),
    camdiag(),
    scHMD(),
    wmm()
{
    ui->setupUi(this);

    if(QApplication::screens().size() <= 1){
        ui->checkBox_screen->setEnabled(false);
        qDebug() << "No 2nd screen attached";
    }

    tmrCam = new QTimer(this);
    connect(tmrCam, SIGNAL(timeout()), this, SLOT(processFrame()));
    tmrCam->start(1000./30.);

    gw = NULL;


    uart_udp = new HAL_UART_UDP;
    linkif_udp = new LinkinterfaceUART_UDP(uart_udp);
    uart = new HAL_UART;
    linkif = new LinkinterfaceUART(uart);

    connect(&diag, SIGNAL(initConnection(bool, QStringList)), this, SLOT(initGateway(bool,QStringList)));
    connect(&diag, SIGNAL(disconnect(bool)), this, SLOT(stopGateway(bool)));

    connect(&camdiag, SIGNAL(openDev(int)), this, SLOT(openDev(int)));
    connect(&camdiag, SIGNAL(closeDev()), this, SLOT(closeDev()));

    connect(this, SIGNAL(hmdHeight(float)), &scHMD, SLOT(setHeight(float)));
    connect(this, SIGNAL(setHeading(float)), &scHMD, SLOT(setHeading(float)));
    connect(this, SIGNAL(setShipDirection(double, double)), &scHMD, SLOT(setHomeDirection(double, double)));
    connect(this, SIGNAL(startFlightTime()), &scHMD, SLOT(startTimer()));
    connect(this, SIGNAL(stopFlightTime()), &scHMD, SLOT(stopTimer()));


    QPixmap i8Map("img/info8.jpg");
    i8Map = i8Map.scaled(40,40,Qt::KeepAspectRatio);
    ui->label_i8->setPixmap(i8Map);
    ui->label_i8->setMask(i8Map.mask());
    ui->label_i8->show();

    QPixmap robexMap("img/robex.png");
    robexMap = robexMap.scaled(40,40,Qt::KeepAspectRatio);
    ui->label_robex->setPixmap(robexMap);
    ui->label_robex->setMask(robexMap.mask());
    ui->label_robex->show();

    QString myStyleSheet =
            "QProgressBar {"
            "border: 2px solid grey;"
            "border-radius: 5px;"
            "text-align: center;"
            "font: bold 20px;"
            "}"

            "QProgressBar::chunk {"
            "background-color: white;"
            "margin: 0.5px;"
            "width: 10px"
            "}";
    ui->batteryStatus->setStyleSheet(myStyleSheet);

    glWidget = new GLWidget;
    QHBoxLayout *hlayout = new QHBoxLayout;
    ui->widget_tr->setLayout(hlayout);
    hlayout->addWidget(glWidget);

    lastIndex = ui->comboBox->currentIndex();

#ifdef OT
    ot = new OpticalTracking(this);
    ot->start();
    connect(ot, SIGNAL(ot_attitude(double,double,double)), this, SLOT(get_OT_Attitude(double,double,double)));
#endif


    polltmr = new QTimer(this);
    connect(polltmr, SIGNAL(timeout()), this, SLOT(pollGateway()));

    timeoutTmr = new QTimer(this);
    connect(timeoutTmr, SIGNAL(timeout()), this, SLOT(resendMsg()));

    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectLegend | QCP::iSelectAxes | QCP::iSelectPlottables);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    ui->customPlot->legend->setVisible(true);
    ui->customPlot->legend->setFont(legendFont);
    ui->customPlot->legend->setSelectedFont(legendFont);
    ui->customPlot->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items
    ui->customPlot->xAxis->setRange(0 - xAxis, 0 + xAxis);
    ui->customPlot->yAxis->setRange(-yAxis, yAxis);

    unsigned int i, graphID = 0;
    for(i = 0; i < sizeof(myTopics) / sizeof(Topic); i++){
        TopicItem* topicItem = new TopicItem();
        topicItem->setText(myTopics[i].name);
        topicItem->setFlags(topicItem->flags() | Qt::ItemIsUserCheckable);
        topicItem->setCheckState(Qt::Unchecked);
        ui->listWidget_Topics->addItem(topicItem);
        QString labels(myTopics[i].labels);
        foreach (QString s,  labels.split(",")){
            PlotItem* item = new PlotItem();
            item->setText(s);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Unchecked);
            item->setGraphID(graphID);

            topicItem->addField(item);

            ui->listWidget->addItem(item);
            ui->customPlot->addGraph();
            ui->customPlot->graph(graphID)->setName(s);
            ui->customPlot->graph(graphID)->setVisible(false);
            ui->customPlot->graph(graphID)->removeFromLegend();

            graphID++;
        }
        topicItem->setFieldsHidden(true);
    }
    ui->customPlot->addGraph();
    ui->customPlot->graph(graphID)->removeFromLegend();
    ui->customPlot->addGraph();
    ui->customPlot->graph(graphID+1)->removeFromLegend();

    start = QTime::currentTime().msecsSinceStartOfDay() / 1000.;
    connect(ui->customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
    connect(ui->listWidget_Topics, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(sendSelectedTopics()));
    connect(this, SIGNAL(imuChanged(float,float,float)), glWidget, SLOT(setEuler(float,float,float)));
    qDebug() << "init done";

    videoLayoutBig = new QGridLayout();
    smallLayout = new QGridLayout();
    mapLayoutBig = new QGridLayout();
    ui->widget_video->setLayout(videoLayoutBig);
    ui->widget_small->setLayout(smallLayout);
    videoView = new QLabel();

    map = new Mapplot();
    ui->widget_gps->setLayout(mapLayoutBig);
    mapLayoutBig->addWidget(map);
}


MainWindow::~MainWindow()
{
#ifdef OT
    ot->quit();
#endif
    scHMD.close();
    delete ui;
}

int cnt = 0;
bool windowOpen = false;
double gas;
RODOS::YPR ypr;
Attitude_t* attitude_state;
Sensor_GPS_t* gps;
Sensor_Voltage_t* battery;
Flight_states_t* state;

bool MainWindow::putGeneric(const long topicId, const unsigned int len, const void* msg, const NetMsgInfo& netMsgInfo){

//    qDebug() << topicId;
#ifdef LIMITED_DATA_RATE
    parseTopicLimitedData(topicId, len, msg, netMsgInfo);
#else
    parseTopicData(topicId, len, msg, netMsgInfo);
#endif
    return true;
}

void MainWindow::pollGateway(){

    if(!ui->checkBox_stop->isChecked()){
        ui->customPlot->xAxis->moveRange(0.02);
        ui->customPlot->replot();
    }
    gw->pollMessages();
    polltmr->start(15);
}

void MainWindow::initGateway(bool serial, QStringList data)
{
    polltmr->stop();
    qDebug() << data;
    if(serial){
        uart->init(data.at(0).toLocal8Bit().data(),data.at(1).toInt());
        gw = new Gateway(linkif);
    } else {
        uart_udp->init(data.at(0).toLocal8Bit().data(),data.at(1).toLocal8Bit().data(),data.at(2).toInt());
        gw = new Gateway(linkif_udp);
    }
    gw->init();
    gw->setPutter(this);
    bearing = 0;
    gw->sendNetworkMessage((char*)&bearing,sizeof(bearing),CORRECT_BEARING_TID ,NOW());
    timeoutTmr->start(1000);
    polltmr->start(15);
    sendSelectedTopics();
}

void MainWindow::stopGateway(bool serial)
{
    polltmr->stop();
    if(serial){
        uart->reset();
    } else {
        uart_udp->reset();
    }
}

void MainWindow::openDev(int i)
{
    capVideo.open(i + CV_CAP_V4L);
}

void MainWindow::closeDev()
{
    capVideo.release();
}

void MainWindow::resendMsg()
{
    gw->sendNetworkMessage((char*)&bearing,sizeof(bearing),CORRECT_BEARING_TID ,NOW());
    timeoutTmr->start(1000);
}

void MainWindow::on_setOPID_clicked(){
    int idx = ui->comboBox->currentIndex();
    on_comboBox_currentIndexChanged(idx);
    if(gw != NULL)
        gw->sendNetworkMessage((char*)&rpy[idx],sizeof(rpy[idx]),SET_PID_VALUES_TID + idx,NOW());
}

void MainWindow::on_pushButton_start_clicked(){
    double button = ACTIVE;
    if(gw != NULL)
        gw->sendNetworkMessage((char*)&button,sizeof(button),ENGINE_CTRL_TID,NOW());
    ui->pushButton_start->setEnabled(false);
    ui->pushButton_stop->setEnabled(true);
}

void MainWindow::on_pushButton_stop_clicked(){
    double button = 0;
    if(gw != NULL)
        gw->sendNetworkMessage((char*)&button,sizeof(button),ENGINE_CTRL_TID,NOW());
    ui->pushButton_start->setEnabled(true);
    ui->pushButton_stop->setEnabled(false);
}

void MainWindow::on_pushButton_speed_clicked()
{
    double speed = (double) ui->speed->value();
    gas = ui->speed->value();
    if(gw != NULL)
        gw->sendNetworkMessage((char*)&speed,sizeof(speed),DESIRED_THRUST_TID,NOW());
}

void MainWindow::on_pushButton_load_clicked(){
    windowOpen = true;
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Save PID Values"), "",
                                                    tr("PID Values (*.pid)"));
    if (fileName.isEmpty()){
        windowOpen = false;
        return;
    }else {

        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }
        //        file.read(5 + sizeof(PID_values_t));
        QByteArray data = file.read(4);
        QString start(data);
        if(start.compare("PID:") == 0){
            for(int index = 0; index < 3; index++){
                data = file.read(sizeof(PID_values_t));
                PID_values_t PID = (*(PID_values_t *) data.data());
                rpy[index] = PID;
                if(index == ui->comboBox->currentIndex()){
                    ui->doubleSpinBox_inP->setValue(rpy[index].inner.x);
                    ui->doubleSpinBox_inI->setValue(rpy[index].inner.y);
                    ui->doubleSpinBox_inD->setValue(rpy[index].inner.z);
                    ui->doubleSpinBox_outP->setValue(rpy[index].outer.x);
                    ui->doubleSpinBox_outI->setValue(rpy[index].outer.y);
                    ui->doubleSpinBox_outD->setValue(rpy[index].outer.z);
                    ui->doubleSpinBox_outLimit->setValue(rpy[index].outerLimit);
                    ui->doubleSpinBox_filterW->setValue(rpy[index].omegaAlpha);
                    ui->doubleSpinBox_filterDeltaW->setValue(rpy[index].deltaOmegaAlpha);
                }
            }
        } else {
            QMessageBox::information(this, tr("Wrong file format"),
                                     file.errorString());
        }
    }
    windowOpen = false;
}

void MainWindow::on_pushButton_save_clicked(){
    windowOpen = true;
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save PID Values"), "",
                                                    tr("PID Values (*.pid)"));
    if (fileName.isEmpty()){
        windowOpen = false;
        return;
    } else {
        if(!fileName.endsWith(".pid")){
            fileName.append(".pid");
        }

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }


        int idx = ui->comboBox->currentIndex();
        rpy[idx].inner = RODOS::Vector3D(ui->doubleSpinBox_inP->value(), ui->doubleSpinBox_inI->value(), ui->doubleSpinBox_inD->value());
        rpy[idx].outer = RODOS::Vector3D(ui->doubleSpinBox_outP->value(), ui->doubleSpinBox_outI->value(), ui->doubleSpinBox_outD->value());
        rpy[idx].outerLimit = ui->doubleSpinBox_outLimit->value();
        rpy[idx].omegaAlpha = ui->doubleSpinBox_filterW->value();
        rpy[idx].deltaOmegaAlpha = ui->doubleSpinBox_filterDeltaW->value();


        char start[] = "PID:";
        file.write(start,4);
        for(idx = 0; idx < 3; idx++){
            //            qDebug() << idx << rpy[idx].outer.x;
            file.write((char*)&rpy[idx],sizeof(rpy[idx]));
        }
        file.close();
    }
    windowOpen = false;
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter){
        char button = 0;
        if(gw != NULL)
            gw->sendNetworkMessage((char*)&button,sizeof(button),ENGINE_CTRL_TID,NOW());
        ui->pushButton_start->setEnabled(true);
        ui->pushButton_stop->setEnabled(false);
    }
}

void MainWindow::parseTopicData(const long topicId, const unsigned int len, const void *msg, const NetMsgInfo &netMsgInfo) {
    double *data = (double *)msg;

    //    qDebug() << topicId;
    long alive = topicId;
    char dat[100];
    //    int length = gw.createNetworkMessage((char*)&alive,sizeof(alive),HEARTBEAT_TID,NOW(),(char*) dat);
    //    uart.write(dat, length);

    if(topicId == BAT_TID){

        battery = (Sensor_Voltage_t*) msg;

        // Battery Color
        QString myStyleSheet =
                "QProgressBar {"
                "border: 2px solid grey;"
                "border-radius: 5px;"
                "text-align: center;"
                "font: bold 20px;"
                "}"

                "QProgressBar::chunk {"
                "background-color:";
        if(battery->percent < 0.3)
            myStyleSheet.append(" red;");
        else if(battery->percent < 0.65)
            myStyleSheet.append(" orange;");
        else
            myStyleSheet.append(" Lime;");


        myStyleSheet.append("margin: 0.5px;"
                            "width: 10px"
                            " }");

        ui->batteryStatus->setStyleSheet(myStyleSheet);
        ui->batteryStatus->setValue(battery->percent * 100);

    }

    if(topicId == STATE_TID){
        state = (Flight_states_t*) msg;
        //        qDebug() << "State" << sizeof(Flight_states_t) << (signed long) *state << MOTOR_OFF;

        switch (*state) {
        case MOTOR_OFF: // = 0, ARMED, TAKE_OFF, ALTITUDE_HOLD, MANUAL_FLIGHT, LANDING:
            ui->flight_state->setText("MOTOR OFF");
            break;
        case ARMED:
            ui->flight_state->setText("ARMED");
            break;
        case TAKE_OFF:
            ui->flight_state->setText("TAKE OFF");
            break;
        case ALTITUDE_HOLD:
            ui->flight_state->setText("ALTITUDE HOLD");
            break;
        case MANUAL_FLIGHT:
            ui->flight_state->setText("MANUAL FLIGHT");
            break;
        case LANDING:
            ui->flight_state->setText("LANDING");
            break;
        default:
            break;
        }
    }

    if(topicId == ATTITUDE_TID){
        attitude_state = (Attitude_t*) msg;
        ypr = attitude_state->q.toYPR();
    } else if (ui->tabWidget_2->currentIndex() == 0){
        return;
    }

    if(topicId == GPS_TID){
        gps = (Sensor_GPS_t*) msg;
        map->setPoseDrone(gps->NED.y, gps->NED.x, ypr.yaw);
    }



    double time = QTime::currentTime().msecsSinceStartOfDay() / 1000. - start;
    if(!windowOpen){

        if(ui->tabWidget_2->currentIndex() == 0){
            emit imuChanged(ypr.roll * 180 / M_PI, ypr.pitch  * 180 / M_PI, (ypr.yaw  * 180 / M_PI) - MY_NORTH);
            ui->q0->display(attitude_state->q.q0);
            ui->q1->display(attitude_state->q.q.x);
            ui->q2->display(attitude_state->q.q.y);
            ui->q3->display(attitude_state->q.q.z);
            ui->roll->display(ypr.roll * 180 / M_PI);
            ui->pitch->display(ypr.pitch * 180 / M_PI);
            ui->yaw->display(ypr.yaw * 180 / M_PI);
            ui->wroll->display(attitude_state->w.x);
            ui->wpitch->display(attitude_state->w.y);
            ui->wyaw->display(attitude_state->w.z);
        } else {
            // For all available topics
            for(int i = 0; i < sizeof(myTopics) / sizeof(Topic); i++){
                // check if received topic ID is valid
                if(topicId == myTopics[i].id){

                    // select TopicItem from List
                    TopicItem* topicItem = (TopicItem*) ui->listWidget_Topics->item(i);
                    int itemCnt = topicItem->size();

                    // for each field of the TopicItem
                    for(int j = 0; j < itemCnt; j++){

                        // check if the checkbox is checked
                        int graphID = topicItem->fields.at(j)->graphID;

                        if(topicItem->fields.at(j)->checkState() == Qt::Checked){
                            // if the color of the PlotItem is white, it wasn't checked before
                            if(ui->listWidget->item(graphID)->backgroundColor().name().compare("#ffffff") == 0){
                                QColor mycolor(colorplate[qrand() % 58]);
                                ui->listWidget->item(graphID)->setBackgroundColor(mycolor);
                                ui->customPlot->graph(graphID)->setPen(QPen(mycolor));
                                ui->customPlot->graph(graphID)->addToLegend();
                            }
                            if(topicId == ATTITUDE_TID && j > 6){
                                switch (j) {
                                case 7:
                                    ui->customPlot->graph(graphID)->addData(time, ypr.roll * 180 / M_PI);
                                    break;
                                case 8:
                                    ui->customPlot->graph(graphID)->addData(time, ypr.pitch * 180 / M_PI);
                                    break;
                                case 9:
                                    ui->customPlot->graph(graphID)->addData(time, ypr.yaw * 180 / M_PI);
                                    break;
                                }
                            } else if(topicId == DESIRED_YPR_TID + OFFSET){
                                ui->customPlot->graph(graphID)->addData(time, data[j]*180/M_PI);
                            } else {
                                ui->customPlot->graph(graphID)->addData(time, data[j]);
                            }
                            ui->customPlot->graph(graphID)->setVisible(true);
                        } else {
                            if(ui->listWidget->item(graphID)->backgroundColor().name().compare("#ffffff") != 0){
                                ui->customPlot->graph(graphID)->setVisible(false);
                                ui->customPlot->graph(graphID)->removeFromLegend();
                                ui->listWidget->item(graphID)->setBackgroundColor(QColor("white"));
                                ui->customPlot->graph(graphID)->setPen(QPen(QColor("black")));
                            }
                        }
                    }
                    // Euler special case + GUI
                    ui->customPlot->replot();
                }
            }
            if(ui->checkBox_tolerance->isChecked()){
                ui->customPlot->graph(ui->customPlot->graphCount()-2)->setVisible(true);
                ui->customPlot->graph(ui->customPlot->graphCount()-1)->setVisible(true);
                ui->customPlot->graph(ui->customPlot->graphCount()-2)->setPen(QPen(Qt::red));
                ui->customPlot->graph(ui->customPlot->graphCount()-1)->setPen(QPen(Qt::red));
                ui->customPlot->graph(ui->customPlot->graphCount()-2)->setBrush(QBrush(QColor(240, 255, 200, 100)));
                ui->customPlot->graph(ui->customPlot->graphCount()-2)->setChannelFillGraph(ui->customPlot->graph(ui->customPlot->graphCount()-1));
                ui->customPlot->graph(ui->customPlot->graphCount()-2)->addData(time, 2);
                ui->customPlot->graph(ui->customPlot->graphCount()-1)->addData(time, -2);
            } else {
                ui->customPlot->graph(ui->customPlot->graphCount()-2)->setVisible(false);
                ui->customPlot->graph(ui->customPlot->graphCount()-1)->setVisible(false);
            }
        }
    }

}

void MainWindow::parseTopicLimitedData(const long topicId, const unsigned int len, const void *msg, const NetMsgInfo &netMsgInfo)
{
    static bool clk_on = false;
    System_State_t *state = (System_State_t*) msg;
    double time = QTime::currentTime().msecsSinceStartOfDay() / 1000. - start;

    // Battery Color
    QString myStyleSheet =
            "QProgressBar {"
            "border: 2px solid grey;"
            "border-radius: 5px;"
            "text-align: center;"
            "font: bold 20px;"
            "}"

            "QProgressBar::chunk {"
            "background-color:";
    if(state->battery.percent < 0.3)
        myStyleSheet.append(" red;");
    else if(state->battery.percent  < 0.65)
        myStyleSheet.append(" orange;");
    else
        myStyleSheet.append(" Lime;");
    myStyleSheet.append("margin: 0.5px;"
                        "width: 10px"
                        " }");

    ui->batteryStatus->setStyleSheet(myStyleSheet);
    ui->batteryStatus->setValue(state->battery.percent  * 100);

    switch (state->state) {
    case MOTOR_OFF: // = 0, ARMED, TAKE_OFF, ALTITUDE_HOLD, MANUAL_FLIGHT, LANDING:
        ui->flight_state->setText("MOTOR OFF");
        if(clk_on){
            emit stopFlightTime();
            clk_on = false;
        }
        break;
    case ARMED:
        ui->flight_state->setText("ARMED");
        break;
    case TAKE_OFF:
        ui->flight_state->setText("TAKE OFF");
        break;
    case ALTITUDE_HOLD:
        ui->flight_state->setText("ALTITUDE HOLD");
        break;
    case MANUAL_FLIGHT:
        ui->flight_state->setText("MANUAL FLIGHT");
        if(!clk_on){
            emit startFlightTime();
            clk_on = true;
        }
        break;
    case LANDING:
        ui->flight_state->setText("LANDING");
        break;
    default:
        break;
    }

    // Calculate YPR
    Quaternion q(state->quat.q0, state->quat.q.x, state->quat.q.y, state->quat.q.z);
    YPR ypr = q.toYPR();

    // Update graphic
    ui->q0->display(state->quat.q0);
    ui->q1->display(state->quat.q.x);
    ui->q2->display(state->quat.q.y);
    ui->q3->display(state->quat.q.z);
    ui->roll->display(ypr.roll * 180 / M_PI);
    ui->pitch->display(ypr.pitch * 180 / M_PI);
    ui->yaw->display(ypr.yaw * 180 / M_PI);

    ui->wroll->setEnabled(false);
    ui->wpitch->setEnabled(false);
    ui->wyaw->setEnabled(false);

    // update 3D view
    emit imuChanged(ypr.roll * 180 / M_PI, ypr.pitch  * 180 / M_PI, (ypr.yaw  * 180 / M_PI) - MY_NORTH);

    // Update Compass on HMD
    emit setHeading(ypr.yaw);
    emit setShipDirection(state->gps.NED.x, state->gps.NED.y);

    // update Height on HMD
    emit hmdHeight(state->lidar.height);

    // update map
    map->setPoseDrone(state->gps.NED.y, state->gps.NED.x, ypr.yaw);

    // Calculate Correct Bearing
    if(state->gps.lat == 0 && state->gps.lon == 0){
        bearing = 0;
        ui->flight_state->setText("NO GPS FIX");
    } else {
        bearing = wmm.getMagDeclination(state->gps.lat, state->gps.lon, state->gps.height) / 180. * M_PI;
//        qDebug() << "Bearing correction at[" << state->gps.lat << "," << state->gps.lon << "]:" <<  bearing;
    }
    gw->sendNetworkMessage((char*)&bearing,sizeof(bearing),CORRECT_BEARING_TID ,NOW());
    timeoutTmr->start(1000);
    if(topicId == myTopics[0].id){

        float *data = (float *)msg + 1;

        state = (System_State_t*) msg;
        Quaternion q;
        q.q0 = state->quat.q0;
        q.q.x = state->quat.q.x;
        q.q.y = state->quat.q.y;
        q.q.z = state->quat.q.z;
        YPR ypr = q.toYPR();

        // select TopicItem from List
        TopicItem* topicItem = (TopicItem*) ui->listWidget_Topics->item(0);
        int itemCnt = topicItem->size();

        // for each field of the TopicItem
        for(int j = 0; j < itemCnt; j++){

            // check if the checkbox is checked
            int graphID = topicItem->fields.at(j)->graphID;

            if(topicItem->fields.at(j)->checkState() == Qt::Checked){
                // if the color of the PlotItem is white, it wasn't checked before
                if(ui->listWidget->item(graphID)->backgroundColor().name().compare("#ffffff") == 0){
                    QColor mycolor(colorplate[qrand() % 58]);
                    ui->listWidget->item(graphID)->setBackgroundColor(mycolor);
                    ui->customPlot->graph(graphID)->setPen(QPen(mycolor));
                    ui->customPlot->graph(graphID)->addToLegend();
                }
                ui->customPlot->graph(graphID)->addData(time, data[j]);
                ui->customPlot->graph(graphID)->setVisible(true);
            } else {
                if(ui->listWidget->item(graphID)->backgroundColor().name().compare("#ffffff") != 0){
                    ui->customPlot->graph(graphID)->setVisible(false);
                    ui->customPlot->graph(graphID)->removeFromLegend();
                    ui->listWidget->item(graphID)->setBackgroundColor(QColor("white"));
                    ui->customPlot->graph(graphID)->setPen(QPen(QColor("black")));
                }
            }
        }
        // Euler special case + GUI
        ui->customPlot->replot();
    }


}


void MainWindow::on_pushButton_selectAll_clicked(){
    for(int i = 0; i < ui->listWidget->count(); i++){
        ui->listWidget->item(i)->setCheckState(Qt::Checked);
    }
}

void MainWindow::on_pushButton_deselectAll_clicked(){
    for(int i = 0; i < ui->listWidget->count(); i++){
        ui->listWidget->item(i)->setCheckState(Qt::Unchecked);
    }
}

void MainWindow::on_pushButton_resetTime_clicked(){
    for(int i = 0; i < ui->customPlot->graphCount(); i++){
        ui->customPlot->graph(i)->clearData();
    }
    start = QTime::currentTime().msecsSinceStartOfDay() / 1000.;
    lastTime = 0;
    ui->customPlot->xAxis->setRange(0 - xAxis, 0 + xAxis);
    ui->customPlot->yAxis->setRange(-yAxis, yAxis);
}

void MainWindow::mousePress()
{
    // if an axis is selected, only allow the direction of that axis to be dragged
    // if no axis is selected, both directions may be dragged

    if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->xAxis->orientation());
    else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->yAxis->orientation());
    else
        ui->customPlot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void MainWindow::mouseWheel()
{
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed

    if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->xAxis->orientation());
    else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->yAxis->orientation());
    else
        ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void MainWindow::selectionChanged()
{
    // synchronize selection of graphs with selection of corresponding legend items:
    for (int i=0; i<ui->listWidget->count(); ++i){
        QCPGraph *graph = ui->customPlot->graph(i);
        if(graph->visible()){
            QCPPlottableLegendItem *item = ui->customPlot->legend->itemWithPlottable(graph);
            if (item->selected() || graph->selected())
            {
                item->setSelected(true);
                graph->setSelected(true);
            }
        }
    }
}

void MainWindow::sendSelectedTopics()
{
    TopicListReport topicIDs;
    cntSelectedTopics = 0;
    for(int i = 0; i < ui->listWidget_Topics->count(); i++){
        TopicItem* item = (TopicItem*) ui->listWidget_Topics->item(i);
        if(item->checkState() == Qt::Checked){
            cntSelectedTopics++;
            item->setFieldsHidden(false);
            topicIDs.add(myTopics[i].id);
        } else {
            item->setFieldsHidden(true);
            for(int j = 0; j < item->fields.size(); j++){
                item->fields.at(j)->setCheckState(Qt::Unchecked);
                ui->customPlot->graph(item->fields.at(j)->graphID)->setVisible(false);
            }
        }
    }
    ui->customPlot->replot();
    if(gw != NULL)
        gw->sendNetworkMessage((char*)&topicIDs,topicIDs.numberOfBytesToSend(),TOPICS_TO_FORWARD_TID,NOW());
}

int precnt = 0;
void MainWindow::get_OT_Attitude(double roll, double pitch, double yaw)
{
    double tmp[] = {roll * 180 / M_PI, pitch * 180 / M_PI, yaw * 180 / M_PI};
    if(precnt == 5) {
        precnt = 0;
        NetMsgInfo info;
        putGeneric(OT_TID, 0, (void*) tmp, info);
    }
    precnt++;
}

void MainWindow::on_pushButton_saveSelected_clicked(){
    windowOpen = true;
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save received data"), "",
                                                    tr("Comma Separeted Values (*.csv)"));
    if (fileName.isEmpty()){
        windowOpen = false;
        return;
    } else {
        //        std::vector<std::vector<double> > data;
        for(int i = 0; i < ui->listWidget->count(); i++){
            if(ui->listWidget->item(i)->checkState() == Qt::Checked){
                qDebug() << ui->listWidget->item(i)->text();
                if(fileName.endsWith(".csv")){
                    fileName.chop(4);
                }

                QFile file(fileName + "_" + ui->customPlot->graph(i)->name() + ".csv");
                if (!file.open(QIODevice::WriteOnly)) {
                    QMessageBox::information(this, tr("Unable to open file"),
                                             file.errorString());
                    return;
                }

                file.write((QString("Time_") + ui->customPlot->graph(i)->name() +", " + ui->customPlot->graph(i)->name() + "\n").toUtf8());
                QCPDataMap *mapData = ui->customPlot->graph(i)->data();
                for(int j = 0; j < mapData->keys().size(); j++){
                    file.write((QString::number(mapData->values().at(j).key)+", ").toUtf8());
                    file.write((QString::number(mapData->values().at(j).value)+"\n").toUtf8());
                }
            }
        }
    }
    windowOpen = false;
}

void MainWindow::on_pushButton_plot_clicked(){
    windowOpen = true;
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save plot"), "",
                                                    tr("Bitmap (*.bmp);;JPG (.jpg);;PNG (.png);;PDF (.pdf)"));
    if (fileName.isEmpty()){
        windowOpen = false;
        return;
    } else {
        if(fileName.endsWith(".bmp")){
            ui->customPlot->saveBmp(fileName, ui->customPlot->width(), ui->customPlot->height());
        } else if(fileName.endsWith(".jpg")){
            ui->customPlot->saveJpg(fileName, ui->customPlot->width(), ui->customPlot->height());
        } else if(fileName.endsWith(".png")){
            ui->customPlot->savePng(fileName, ui->customPlot->width(), ui->customPlot->height());
        } else if(fileName.endsWith(".pdf")){
            ui->customPlot->savePdf(fileName, ui->customPlot->width(), ui->customPlot->height());
        } else {
            fileName.append(".png");
            ui->customPlot->savePng(fileName, ui->customPlot->width(), ui->customPlot->height());
        }
    }
    windowOpen = false;

}

void MainWindow::on_pushButton_setRPY_clicked(){
    double roll = ui->spinBox_desRoll->value() / 180. * M_PI;
    double pitch = ui->spinBox_desPitch->value() / 180. * M_PI;
    double yaw = ui->spinBox_desYaw->value() / 180. * M_PI;
    RODOS::YPR ypr(yaw,pitch,roll);
    if(gw != NULL)
        gw->sendNetworkMessage((char*)&ypr,sizeof(ypr),DESIRED_YPR_TID,NOW());
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    rpy[lastIndex].inner = RODOS::Vector3D(ui->doubleSpinBox_inP->value(), ui->doubleSpinBox_inI->value(), ui->doubleSpinBox_inD->value());
    rpy[lastIndex].outer = RODOS::Vector3D(ui->doubleSpinBox_outP->value(), ui->doubleSpinBox_outI->value(), ui->doubleSpinBox_outD->value());
    rpy[lastIndex].outerLimit = ui->doubleSpinBox_outLimit->value();
    rpy[lastIndex].omegaAlpha = ui->doubleSpinBox_filterW->value();
    rpy[lastIndex].deltaOmegaAlpha = ui->doubleSpinBox_filterDeltaW->value();

    lastIndex = index;

    ui->doubleSpinBox_inP->setValue(rpy[index].inner.x);
    ui->doubleSpinBox_inI->setValue(rpy[index].inner.y);
    ui->doubleSpinBox_inD->setValue(rpy[index].inner.z);
    ui->doubleSpinBox_outP->setValue(rpy[index].outer.x);
    ui->doubleSpinBox_outI->setValue(rpy[index].outer.y);
    ui->doubleSpinBox_outD->setValue(rpy[index].outer.z);
    ui->doubleSpinBox_outLimit->setValue(rpy[index].outerLimit);
    ui->doubleSpinBox_filterW->setValue(rpy[index].omegaAlpha);
    ui->doubleSpinBox_filterDeltaW->setValue(rpy[index].deltaOmegaAlpha);

}

void MainWindow::on_pushButton_save_alt_clicked()
{
    windowOpen = true;
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save PIDH Values"), "",
                                                    tr("PIDH Values (*.pidh)"));
    if (fileName.isEmpty()){
        windowOpen = false;
        return;
    } else {
        if(!fileName.endsWith(".pidh")){
            fileName.append(".pidh");
        }

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }

        PID_values_t altPID;
        altPID.inner = RODOS::Vector3D(ui->doubleSpinBox_inP_alt->value(), ui->doubleSpinBox_inI_alt->value(), ui->doubleSpinBox_inD_alt->value());
        altPID.outer = RODOS::Vector3D(ui->doubleSpinBox_outP_alt->value(), ui->doubleSpinBox_outI_alt->value(), ui->doubleSpinBox_outD_alt->value());
        altPID.outerLimit = ui->doubleSpinBox_outLimit_alt->value();
        altPID.omegaAlpha = ui->doubleSpinBox_filterW_alt->value();
        altPID.deltaOmegaAlpha = ui->doubleSpinBox_filterDeltaW_alt->value();

        char start[] = "PID_ALT:";
        file.write(start,8);
        file.write((char*)&altPID,sizeof(altPID));
        file.close();
    }
    windowOpen = false;
}

void MainWindow::on_pushButton_load_alt_clicked()
{
    windowOpen = true;
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Save PIDH Values"), "",
                                                    tr("PIDH Values (*.pidh)"));
    if (fileName.isEmpty()){
        windowOpen = false;
        return;
    }else {

        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }
        //        file.read(5 + sizeof(PID_values_t));
        QByteArray data = file.read(8);
        QString start(data);
        if(start.compare("PID_ALT:") == 0){
            data = file.read(sizeof(PID_values_t));
            PID_values_t PID = (*(PID_values_t *) data.data());
            ui->doubleSpinBox_inP_alt->setValue(PID.inner.x);
            ui->doubleSpinBox_inI_alt->setValue(PID.inner.y);
            ui->doubleSpinBox_inD_alt->setValue(PID.inner.z);
            ui->doubleSpinBox_outP_alt->setValue(PID.outer.x);
            ui->doubleSpinBox_outI_alt->setValue(PID.outer.y);
            ui->doubleSpinBox_outD_alt->setValue(PID.outer.z);
            ui->doubleSpinBox_outLimit_alt->setValue(PID.outerLimit);
            ui->doubleSpinBox_filterW_alt->setValue(PID.omegaAlpha);
            ui->doubleSpinBox_filterDeltaW_alt->setValue(PID.deltaOmegaAlpha);
        } else {
            QMessageBox::information(this, tr("Wrong file format"),
                                     file.errorString());
        }
    }
    windowOpen = false;
}

void MainWindow::on_setOPID_alt_clicked()
{
    PID_values_t altPID;
    altPID.inner = RODOS::Vector3D(ui->doubleSpinBox_inP_alt->value(), ui->doubleSpinBox_inI_alt->value(), ui->doubleSpinBox_inD_alt->value());
    altPID.outer = RODOS::Vector3D(ui->doubleSpinBox_outP_alt->value(), ui->doubleSpinBox_outI_alt->value(), ui->doubleSpinBox_outD_alt->value());
    altPID.outerLimit = ui->doubleSpinBox_outLimit_alt->value();
    altPID.omegaAlpha = ui->doubleSpinBox_filterW_alt->value();
    altPID.deltaOmegaAlpha = ui->doubleSpinBox_filterDeltaW_alt->value();
    if(gw != NULL)
        gw->sendNetworkMessage((char*)&altPID,sizeof(altPID),SET_PID_ALT_VALUES_TID ,NOW());
}

void MainWindow::on_actionConnect_triggered(bool checked)
{
    diag.exec();
}

void MainWindow::processFrame(){

    if(!capVideo.isOpened()){
        return;
    }

    capVideo.read(frame);
    if(frame.empty()){
        return;
    }

    cv::cvtColor(frame, frame, CV_BGR2RGB);
    QImage img((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

    if(screenActive){
        int w = scHMD.screenWidth();
        int h = scHMD.screenHeight();
        QPixmap scale = QPixmap::fromImage(img).scaled(w,h);
        scHMD.setImagePixmap(scale);
        scHMD.setBatteryStyleSheet(ui->batteryStatus->styleSheet());
        scHMD.setBattery(ui->batteryStatus->value());
        scHMD.setFlightState(ui->flight_state->text());

//        battery2.setStyleSheet(ui->batteryStatus->styleSheet());
//        battery2.setValue(ui->batteryStatus->value());
    }

    if(ui->tabWidget_2->currentIndex() == 2){
        // Show big time
        //        if(!videoLayoutBig->isEmpty())
        videoLayoutBig->removeWidget(videoView);
        if(!smallLayout->isEmpty()) {
            smallLayout->removeWidget(videoView);
            smallLayout->addWidget(map);
            mapLayoutBig->removeWidget(map);
        }
        videoLayoutBig->addWidget(videoView);
        videoView->setPixmap(QPixmap::fromImage(img));
        videoView->setScaledContents(true);
    } else {
        // Show small time
        cv::resize(frame, frame, cv::Size(360,240));
        QImage imgSmall((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        if(!videoLayoutBig->isEmpty())
            videoLayoutBig->removeWidget(videoView);
        if(!smallLayout->isEmpty()){
            smallLayout->removeWidget(videoView);
            smallLayout->removeWidget(map);
            mapLayoutBig->addWidget(map);
        }
        smallLayout->addWidget(videoView);
        videoView->setPixmap(QPixmap::fromImage(imgSmall));
        videoView->setScaledContents(true);
    }



    //    ui->video->setPixmap(QPixmap::fromImage(img));
}


void MainWindow::on_actionCamera_triggered()
{
    camdiag.exec();
}

void MainWindow::on_checkBox_screen_clicked(bool checked)
{
    screenActive = checked;
    if(checked)
        scHMD.showFullScreen();
    else
        scHMD.close();
}


void MainWindow::on_pushButton_resetGPS_clicked()
{
    map->resetGPS();
}

void MainWindow::on_setOPID_pos_clicked()
{
    PID_values_t altPID;
    altPID.outer = RODOS::Vector3D(ui->doubleSpin_posP->value(), ui->doubleSpin_posI->value(), ui->doubleSpin_posD->value());
    altPID.outerLimit = ui->doubleSpin_posLim->value();
    if(gw != NULL)
        gw->sendNetworkMessage((char*)&altPID,sizeof(altPID),SET_PID_POSE_VAL_TID ,NOW());
}

void MainWindow::on_pushButton_save_pos_clicked()
{
    windowOpen = true;
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save PIDP Values"), "",
                                                    tr("PIDP Values (*.pidp)"));
    if (fileName.isEmpty()){
        windowOpen = false;
        return;
    } else {
        if(!fileName.endsWith(".pidp")){
            fileName.append(".pidp");
        }

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }

        PID_values_t POSPID;
        POSPID.outer = RODOS::Vector3D(ui->doubleSpin_posP->value(), ui->doubleSpin_posI->value(), ui->doubleSpin_posD->value());
        POSPID.outerLimit = ui->doubleSpin_posLim->value();

        char start[] = "PID_POS:";
        file.write(start,8);
        file.write((char*)&POSPID,sizeof(POSPID));
        file.close();
    }
    windowOpen = false;
}

void MainWindow::on_pushButton_load_pos_clicked()
{
    windowOpen = true;
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Save PIDP Values"), "",
                                                    tr("PIDP Values (*.pidp)"));
    if (fileName.isEmpty()){
        windowOpen = false;
        return;
    }else {

        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }
        //        file.read(5 + sizeof(PID_values_t));
        QByteArray data = file.read(8);
        QString start(data);
        if(start.compare("PID_POS:") == 0){
            data = file.read(sizeof(PID_values_t));
            PID_values_t PID = (*(PID_values_t *) data.data());
            ui->doubleSpin_posP->setValue(PID.outer.x);
            ui->doubleSpin_posI->setValue(PID.outer.y);
            ui->doubleSpin_posD->setValue(PID.outer.z);
            ui->doubleSpin_posLim->setValue(PID.outerLimit);
        } else {
            QMessageBox::information(this, tr("Wrong file format"),
                                     file.errorString());
        }
    }
    windowOpen = false;
}


void MainWindow::on_set_Goal_Pose_clicked()
{
    Pose_t pose(ui->doubleSpinBox_Pose_1->value(), ui->doubleSpinBox_Pose_2->value(), ui->doubleSpinBox_Pose_heading->value() / 180. * M_PI);
    pose.frame = (frame_t) ui->frameSelect->currentIndex();
    if(gw != NULL)
        gw->sendNetworkMessage((char*)&pose,sizeof(pose),DESIRED_POSE_TID ,NOW());
}

void MainWindow::on_frameSelect_currentIndexChanged(int index)
{
    switch (index) {
    case BODY:
        ui->label_pos1->setText("x");
        ui->label_pos2->setText("y");
        break;
    case NED:
        ui->label_pos1->setText("North");
        ui->label_pos2->setText("East");
        break;
    case GEO:
        ui->label_pos1->setText("Lat");
        ui->label_pos2->setText("Lon");
        break;
    default:
        break;
    }
}
