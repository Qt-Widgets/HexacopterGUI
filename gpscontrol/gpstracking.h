#ifndef GPSTRACKING_H
#define GPSTRACKING_H

#include "ui_mainwindow.h"
#include "gpscoordinate.h"
#include <QtWebKit>
#include <QTimer>
#include <QWebView>
#include <QList>
#include <QTableWidget>
#include <QFileDialog>
#include <QString>
#include <QPixmap>
#include <QMessageBox>
#include <QVariant>
#include <QWebFrame>
#include <QStringList>
#include <QWebElement>
#include <QNetworkConfigurationManager>


class MainWindow;
class GPSTracking:public QWidget
{
    Q_OBJECT

public:
   GPSTracking(Ui::MainWindow* u);
   ~GPSTracking();
   GPSCoordinate currentCoords;
   QNetworkConfigurationManager *mgr;
private:

    Ui::MainWindow* ui;
    void loadTracking();
    void evalJs(const QString &jsfunction);

 public slots:
    void frameGPSRecieved(double lat, double lon, double height);
    void clearTracking();
    void setJsObject();
};
#endif // GPSTRACKING_H
