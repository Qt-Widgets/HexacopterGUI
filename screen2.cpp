#include "screen2.h"
#include "ui_screen2.h"

#include "QDebug"
#include <QScreen>

Screen2::Screen2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Screen2),
    airTime()
{
    ui->setupUi(this);
    QScreen* screen;
    if(QApplication::screens().size() > 1){
      screen = QApplication::screens().at(1);
    } else {
       screen = QApplication::screens().at(0);
    }
    QRect screenres = screen->geometry();
    this->move(QPoint(screenres.x(), screenres.y()));
    this->resize(screenres.width(), screenres.height());

    QPalette palette = this->palette();
    palette.setColor(this->backgroundRole(), Qt::black);
    this->setPalette(palette);

//    QPalette pText = ui->label_height->palette();
//    palette.setColor(ui->label_height->foregroundRole(), Qt::white);
//    ui->label_height->setPalette(pText);
//    ui->label_state->setPalette(pText);
//    ui->label_time->setPalette(pText);

    mCompassGauge = new QcGaugeWidget;

    QcLabelItem *w = mCompassGauge->addLabel(60);
    w->setText("W");
    w->setAngle(0);
    w->setColor(Qt::black);

    QcLabelItem *n = mCompassGauge->addLabel(60);
    n->setText("N");
    n->setAngle(90);
    n->setColor(Qt::black);

    QcLabelItem *e = mCompassGauge->addLabel(60);
    e->setText("E");
    e->setAngle(180);
    e->setColor(Qt::black);

    QcLabelItem *s = mCompassGauge->addLabel(60);
    s->setText("S");
    s->setAngle(270);
    s->setColor(Qt::black);

    QcDegreesItem *deg = mCompassGauge->addDegrees(50);
    deg->setStep(5);
    deg->setMaxDegree(360);
    deg->setMinDegree(0);
    deg->setColor(Qt::black);

    lab = mCompassGauge->addLabel(75);

    mShipNeedle = mCompassGauge->addNeedle(55);
    mShipNeedle->setNeedle(QcNeedleItem::AttitudeMeterNeedle);
    mShipNeedle->setValueRange(-360,360);
    mShipNeedle->setMaxDegree(360);
    mShipNeedle->setMinDegree(-360);
    mShipNeedle->setColor(Qt::green);

    mHeadingNeedle = mCompassGauge->addNeedle(45);
    mHeadingNeedle->setNeedle(QcNeedleItem::DiamonNeedle);
    mHeadingNeedle->setValueRange(0,360);
    mHeadingNeedle->setMaxDegree(360);
    mHeadingNeedle->setMinDegree(0);
    mHeadingNeedle->setColor(Qt::red);

    ui->compassLayout->addWidget(mCompassGauge);


    time = new QTime;
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    timer->start(1000);

    hr = 0;
    min = 0;
    sec = 0;
    oldmsec = 0;
    timerOn = false;
    airTime.setHMS(hr, min, sec);
}

Screen2::~Screen2()
{
    delete ui;
}

int Screen2::screenWidth()
{
    return ui->image->width();
}

int Screen2::screenHeight()
{
    return ui->image->height();
}

void Screen2::setFlightState(QString state)
{
    ui->label_state->setText("<font color=white>" + state + "</font>");
}

void Screen2::setBatteryStyleSheet(QString styleSheet)
{
    ui->progressBar->setStyleSheet(styleSheet);
}

void Screen2::setBattery(int percent)
{
    ui->progressBar->setValue(percent);
}

void Screen2::setImagePixmap(QPixmap &map)
{
    ui->image->setPixmap(map);
}


void Screen2::setHeight(float height)
{
    ui->label_height->setText("<font color=white>" + QString::number(height / 100., 'f', 2) + " m</font>");
}

void Screen2::setHomeDirection(double x, double y)
{
//    qDebug() << Home Direction << atan2(y,x) * 180. / M_PI - 90;
    mShipNeedle->setCurrentValue(atan2(y,x) * 180. / M_PI - 90);
    lab->setText(QString::number(sqrt(x*x + y*y), 'f', 1) + " m");
}

void Screen2::setHeading(float angle)
{
    mHeadingNeedle->setCurrentValue(angle * 180. / M_PI + 90);
}

void Screen2::startTimer()
{
    timerOn = true;
    time->start();
}

void Screen2::stopTimer()
{
    timerOn = false;
    oldmsec = time->elapsed();
}

void Screen2::updateTime()
{
    if(timerOn){
        airTime.setHMS(0,0,0);
        airTime = airTime.addMSecs(time->elapsed() + oldmsec);
    }
    ui->label_time->setText("<font color=white>" + airTime.toString("mm:ss") +"</font>");
}
