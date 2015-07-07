#ifndef SCREEN2_H
#define SCREEN2_H

#include <QWidget>
#include <QTime>
#include <QTimer>
#include <QcGaugeWidget/qcgaugewidget.h>

namespace Ui {
class Screen2;
}

class Screen2 : public QWidget
{
    Q_OBJECT

public:
    explicit Screen2(QWidget *parent = 0);
    ~Screen2();

    int screenWidth();
    int screenHeight();
    void setFlightState(QString state);
    void setBatteryStyleSheet(QString styleSheet);
    void setBattery(int percent);
    void setImagePixmap(QPixmap &map);



public slots:
    void setHeight(float height);
    void setHomeDirection(double x, double y);
    void setHeading(float angle);
    void startTimer();
    void stopTimer();
    void updateTime();
private:
    Ui::Screen2 *ui;

    QcGaugeWidget * mCompassGauge;
    QcNeedleItem *mHeadingNeedle;
    QcNeedleItem *mShipNeedle;
    QcLabelItem *lab;

    bool timerOn;
    QTime *time;
    QTime airTime;
    QTimer *timer;
    int oldmsec;
    int hr, min, sec;
};

#endif // SCREEN2_H
