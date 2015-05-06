#include "gpstracking.h"


GPSTracking::GPSTracking(Ui::MainWindow* u){
    this->ui=u;

    ui->gpswebView1->load(QUrl("qrc:/gTracking.html"));
    connect(this->ui->gpswebView1->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(setJsObject()));
    connect(ui->gpsClear, SIGNAL(clicked()),this,SLOT(clearTracking()));
}

//setzt ein Objekt zur Kommunikation mit der Js-Klasse - Möglichkeit der Übergabe von Variablen aus Js an die C++-Klasse
void GPSTracking::setJsObject(){
    this->ui->gpswebView1->page()->mainFrame()->addToJavaScriptWindowObject("tracking",this);
}

//handled die Verbindung zwischen der Klasse und dem JavaScript der HTML des WebView
void GPSTracking::evalJs(const QString &jsfunction){
    /*qDebug() << jsfunction <<  */this->ui->gpswebView1->page()->mainFrame()->evaluateJavaScript(jsfunction);
}

//uebergibt eintreffende Koordinaten an das JavaScript mittels der Funktion evalJS
void GPSTracking::frameGPSRecieved(double lat, double lon, double height){
   evalJs(QString("setPath(%1,%2)").arg(QString::number((double)lat, 'f', 7)).arg(QString::number((double)lon, 'f', 7)));
   if(ui->gpsFollow->isChecked()){
       evalJs(QString("setCenter()"));
   }
}

void GPSTracking::clearTracking(){
    evalJs(QString("clear()"));
}

GPSTracking::~GPSTracking()
{

}

