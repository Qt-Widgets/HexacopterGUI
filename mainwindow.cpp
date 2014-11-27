#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "glwidget.h"

#include <QDebug>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidgetItem>

#include "datatypes.h"
#include "matlib.h"

#define MY_NORTH    3.0f //30.8f

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

static QString parameters[] = {"Roll", "Pitch", "Yaw", "q0", "q1", "q2", "q3", "wx", "wy", "wz", "inner PID roll P", "inner PID roll I", "inner PID roll D",
                               "inner PID pitch P", "inner PID pitch I", "inner PID pitch D",
                               "inner PID yaw P", "inner PID yaw I", "inner PID yaw D",
                               "outer PID roll P", "outer PID roll I", "outer PID roll D",
                               "outer PID pitch P", "outer PID pitch I", "outer PID pitch D",
                               "outer PID yaw P", "outer PID yaw I", "outer PID yaw D", "torque roll", "torque pitch", "torque yaw", "error roll", "error pitch", "error yaw"};

static HAL_UART uart(UART_IDX2);    // rfcomm0
static LinkinterfaceUART linkif(&uart);

static double yAxis = 50;
static double xAxis = 15;

int64_t NOW(){
    struct timeval t;
    gettimeofday(&t,0);
    return (t.tv_sec*1000000LL+t.tv_usec)*1000;
}

static Gateway gw(&linkif);

int cnt2=0;

struct Topic {
    uint8_t header [26];
    uint8_t userData [1];
};

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
    Putter()
{
    ui->setupUi(this);
    glWidget = new GLWidget;
    this->setCentralWidget(ui->tabWidget);
    QHBoxLayout *hlayout = new QHBoxLayout;
    ui->widget_tr->setLayout(hlayout);
    hlayout->addWidget(glWidget);

    uart.init(115200);
    gw.init();
    gw.setPutter(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(pollGateway()));
    timer->start(10);


    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectLegend | QCP::iSelectAxes | QCP::iSelectPlottables);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    ui->customPlot->legend->setVisible(true);
    ui->customPlot->legend->setFont(legendFont);
    ui->customPlot->legend->setSelectedFont(legendFont);
    ui->customPlot->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items
    ui->customPlot->xAxis->setRange(0 - xAxis, 0 + xAxis);
    ui->customPlot->yAxis->setRange(-yAxis, yAxis);

    for(int i = 0; i < sizeof(debug_t) / sizeof(double) + 3; i++){
        ui->customPlot->addGraph();
        ui->customPlot->graph(i)->setName(parameters[i]);
        ui->customPlot->graph(i)->removeFromLegend();
        QListWidgetItem* item = new QListWidgetItem();
        item->setText(parameters[i]);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        ui->listWidget->addItem(item);

    }

    start = QTime::currentTime().msecsSinceStartOfDay() / 1000.;

    connect(ui->customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
    connect(this, SIGNAL(imuChanged(float,float,float)), glWidget, SLOT(setEuler(float,float,float)));
    qDebug() << "init done";

}


MainWindow::~MainWindow()
{
    delete ui;
}

int cnt = 0;
bool windowOpen = false;

bool MainWindow::putGeneric(const long topicId, const unsigned int len, const void* msg, const NetMsgInfo& netMsgInfo){
    State_t state;
    debug_t datain;
    double *data;
    RODOS::YPR ypr;
    if(!windowOpen){
        switch(topicId) {
        case 1004:
            datain = (*(debug_t *)msg);
            state = datain.state;
            data = (double *)msg;
            // Calculate RPY from Quaternion
            if(cnt++ == 2){
                cnt = 0;
                ypr = state.q.toYPRnils();

                double time = QTime::currentTime().msecsSinceStartOfDay() / 1000. - start;
                ui->customPlot->xAxis->moveRange(time - lastTime);


                if(ui->tabWidget_2->currentIndex() == 0) {
                    emit imuChanged(ypr.roll * 180 / M_PI, ypr.pitch  * 180 / M_PI, (ypr.yaw  * 180 / M_PI) - MY_NORTH);
                } else {
                    for(int i = 0; i < ui->listWidget->count(); i++){
                        if(ui->listWidget->item(i)->checkState() == Qt::Checked){
                            if(ui->listWidget->item(i)->backgroundColor().name().compare("#ffffff") == 0){
                                //                            QColor mycolor(QColor::colorNames().at(qrand() % QColor::colorNames().size()));
                                QColor mycolor(colorplate[qrand() % 58]);
                                ui->listWidget->item(i)->setBackgroundColor(mycolor);
                                ui->customPlot->graph(i)->setPen(QPen(mycolor));
                                ui->customPlot->graph(i)->addToLegend();
                            }
                            if(i < 3){
                                switch (i) {
                                case 0:
                                    ui->customPlot->graph(0)->addData(time, ypr.roll * 180 / M_PI);
                                    break;
                                case 1:
                                    ui->customPlot->graph(1)->addData(time, ypr.pitch * 180 / M_PI);
                                    break;
                                case 2:
                                    ui->customPlot->graph(2)->addData(time, ypr.yaw * 180 / M_PI);
                                    break;
                                }
                            } else {
                                ui->customPlot->graph(i)->addData(time, data[i-3]);
                            }
                            ui->customPlot->graph(i)->setVisible(true);
                        } else {
                            ui->customPlot->graph(i)->setVisible(false);
                            ui->customPlot->graph(i)->removeFromLegend();
                            ui->listWidget->item(i)->setBackgroundColor(QColor("white"));
                            ui->customPlot->graph(i)->setPen(QPen(QColor("black")));
                        }
                    }
                    ui->customPlot->replot();
                }

                ui->q0->setText(QString::number(state.q.q0, 'f', 3));
                ui->q1->setText(QString::number(state.q.q.x, 'f', 3));
                ui->q2->setText(QString::number(state.q.q.y, 'f', 3));
                ui->q3->setText(QString::number(state.q.q.z, 'f', 3));
                ui->roll->setText(QString::number(ypr.roll * 180 / M_PI, 'f', 3));
                ui->pitch->setText(QString::number(ypr.pitch * 180 / M_PI, 'f', 3));
                ui->yaw->setText(QString::number(ypr.yaw * 180 / M_PI, 'f', 3));
                ui->wroll->setText(QString::number(state.w.x, 'f', 3));
                ui->wpitch->setText(QString::number(state.w.y, 'f', 3));
                ui->wyaw->setText(QString::number(state.w.z, 'f', 3));
                lastTime = time;
            }

            break;
        default:
            break;

        }
    }
    return true;
}

void MainWindow::pollGateway(){
    gw.pollMessages();
    char alive = 217;
    char msg[100];
    int length = gw.createNetworkMessage((char*)&alive,sizeof(alive),2000,NOW(),(char*) msg);
    uart.write(msg, length);
    timer->start(10);
}

void MainWindow::on_setIPID_clicked(){
    PID_values_t PID;
    PID.roll = RODOS::Vector3D(ui->iProll->value(), ui->iIroll->value(), ui->iDroll->value());
    PID.pitch = RODOS::Vector3D(ui->iPpitch->value(), ui->iIpitch->value(), ui->iDpitch->value());
    PID.yaw = RODOS::Vector3D(ui->iPyaw->value(), ui->iIyaw->value(), ui->iDyaw->value());
    // Some buffer
    char msg[100];
    int length = gw.createNetworkMessage((char*)&PID,sizeof(PID),2001,NOW(),(char*) msg);
    uart.write(msg, length);
}

void MainWindow::on_setOPID_clicked(){
    PID_values_t PID;
    PID.roll = RODOS::Vector3D(ui->oProll->value(), ui->oIroll->value(), ui->oDroll->value());
    PID.pitch = RODOS::Vector3D(ui->oPpitch->value(), ui->oIpitch->value(), ui->oDpitch->value());
    PID.yaw = RODOS::Vector3D(ui->oPyaw->value(), ui->oIyaw->value(), ui->oDyaw->value());
    char msg[100];
    int length = gw.createNetworkMessage((char*)&PID,sizeof(PID),2002,NOW(),(char*) msg);
    uart.write(msg, length);
}

void MainWindow::on_pushButton_start_clicked(){
    char button = 217;
    char msg[100];
    int length = gw.createNetworkMessage((char*)&button,sizeof(button),2003,NOW(),(char*) msg);
    uart.write(msg, length);
    ui->pushButton_start->setEnabled(false);
    ui->pushButton_stop->setEnabled(true);
}

void MainWindow::on_pushButton_stop_clicked(){
    char button = 0;
    char msg[100];
    int length = gw.createNetworkMessage((char*)&button,sizeof(button),2003,NOW(),(char*) msg);
    uart.write(msg, length);
    ui->pushButton_start->setEnabled(true);
    ui->pushButton_stop->setEnabled(false);
}

void MainWindow::on_pushButton_speed_clicked()
{
    char speed = (char) ui->speed->value();
    char msg[100];
    int length = gw.createNetworkMessage((char*)&speed,sizeof(speed),2004,NOW(),(char*) msg);
    uart.write(msg, length);
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

        QByteArray data = file.read(5);
        QString start(data);
        if(start.compare("iPID:") == 0){
            data = file.read(72);
            PID_values_t iPID = (*(PID_values_t *) data.data());
            ui->iProll->setValue(iPID.roll.x);
            ui->iIroll->setValue(iPID.roll.y);
            ui->iDroll->setValue(iPID.roll.z);
            ui->iPpitch->setValue(iPID.pitch.x);
            ui->iIpitch->setValue(iPID.pitch.y);
            ui->iDpitch->setValue(iPID.pitch.z);
            ui->iPyaw->setValue(iPID.yaw.x);
            ui->iIyaw->setValue(iPID.yaw.y);
            ui->iDyaw->setValue(iPID.yaw.z);
        }
        data = file.read(5);
        QString split(data);
        if(split.compare("oPID:") == 0){
            data = file.read(72);
            PID_values_t oPID = (*(PID_values_t *) data.data());
            ui->oProll->setValue(oPID.roll.x);
            ui->oIroll->setValue(oPID.roll.y);
            ui->oDroll->setValue(oPID.roll.z);
            ui->oPpitch->setValue(oPID.pitch.x);
            ui->oIpitch->setValue(oPID.pitch.y);
            ui->oDpitch->setValue(oPID.pitch.z);
            ui->oPyaw->setValue(oPID.yaw.x);
            ui->oIyaw->setValue(oPID.yaw.y);
            ui->oDyaw->setValue(oPID.yaw.z);
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

        PID_values_t iPID;
        iPID.roll = RODOS::Vector3D(ui->iProll->value(), ui->iIroll->value(), ui->iDroll->value());
        iPID.pitch = RODOS::Vector3D(ui->iPpitch->value(), ui->iIpitch->value(), ui->iDpitch->value());
        iPID.yaw = RODOS::Vector3D(ui->iPyaw->value(), ui->iIyaw->value(), ui->iDyaw->value());
        PID_values_t oPID;
        oPID.roll = RODOS::Vector3D(ui->oProll->value(), ui->oIroll->value(), ui->oDroll->value());
        oPID.pitch = RODOS::Vector3D(ui->oPpitch->value(), ui->oIpitch->value(), ui->oDpitch->value());
        oPID.yaw = RODOS::Vector3D(ui->oPyaw->value(), ui->oIyaw->value(), ui->oDyaw->value());

        char start[] = "iPID:";
        file.write(start,5);
        file.write((char*)&iPID,sizeof(iPID));
        char sep[] = "oPID:";
        file.write(sep,5);
        file.write((char*)&oPID,sizeof(oPID));
        file.close();
    }
    windowOpen = false;
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter){
        char button = 0;
        char msg[100];
        int length = gw.createNetworkMessage((char*)&button,sizeof(button),2003,NOW(),(char*) msg);
        uart.write(msg, length);
        ui->pushButton_start->setEnabled(true);
        ui->pushButton_stop->setEnabled(false);
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
    for (int i=0; i<ui->customPlot->graphCount(); ++i){
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
