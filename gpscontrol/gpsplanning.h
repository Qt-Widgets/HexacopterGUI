#ifndef GPSPLANNING_H
#define GPSPLANNING_H
#include "ui_mainwindow.h"
#include "../driver/QSerial.h"
#include "gpscoordinate.h"
#include <QWebView>
#include <QList>
#include <QTableWidget>
#include <QFileDialog>
#include <QString>
#include <QPixmap>
#include <QMessageBox>
#include "driver/frame.h"
#include <QVariant>
#include <QWebFrame>
#include <QStringList>
#include <QWebElement>
#include <QRegExp>
#include "gpstracking.h"


class MainWindow;
class GPSPlanning:public QWidget
{
    Q_OBJECT

public:
   GPSPlanning(Ui::MainWindow* u,QmySerial* serial,GPSTracking *tracking);
   ~GPSPlanning();
   QmySerial* serial;
   QList<GPSCoordinate> coordinatePlanning;
   QList<GPSCoordinate> sollList;
   GPSCoordinate currentData;


private:
    int row;
    double latitude;
    double longitude;
    Ui::MainWindow* ui;
    QString dateTimeSysString;
    GPSTracking *tracking;
    void evalJs(const QString &jsfunction);
    void loadDialog();
    void setCoordsView();
    void setSingleCoord();
    void setAllCoords();
    QStandardItemModel* model;

 public slots:
    void savePlanning();
    void loadPlanning();
    void clearPlanning();
    void setJsObject();
    void sendPath();
    void sendCoordinate();
    void receiveAck(float data);
    void showClickedMarker(double latitude, double longitude);
    void getMapCoordinates(qreal latitude,qreal longitude);
    void deleteMapCoordinates(qreal latitude,qreal longitude);
    void handleComboSelectionP(int option);
    void handleString();
    void handleQuadropos();
    void controlAdress(qreal test);
    void copterPos(QList<float>* data);




};
#endif // GPSPLANNING_H
