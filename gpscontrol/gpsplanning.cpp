#include "gpsplanning.h"

GPSPlanning::GPSPlanning(Ui::MainWindow* u,QmySerial* serial,GPSTracking *tracking)
{
    this->ui=u;
    this->serial=serial;
    this->tracking=tracking;
    //Load google maps view for the planning part
    this->ui->gpswebView3->load(QUrl("qrc:/gPlanning.html"));
    this->ui->gpswebView3->settings()->setAttribute( QWebSettings::JavascriptEnabled, true );

    //ensure that your QObjects remain accessible after loading a new URL
    connect(this->ui->gpswebView3->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
                       this, SLOT(setJsObject()));

    ui->gpscomboBox_city->addItem(QString("---"));
    ui->gpscomboBox_city->addItem(QString("Berlin"));
    ui->gpscomboBox_city->addItem(QString("München"));
    ui->gpscomboBox_city->addItem(QString("Nürnberg"));
    ui->gpscomboBox_city->addItem(QString("Frankfurt"));
    ui->gpscomboBox_city->addItem(QString("Würzburg"));

    setCoordsView();

    connect(ui->gpslineEdit,SIGNAL(returnPressed()),this,SLOT(handleString()));
    connect(ui->gpsQuadroPos,SIGNAL(clicked()),this,SLOT(handleQuadropos()));
    connect(ui->gpsButton_new,SIGNAL(clicked()),this,SLOT(clearPlanning()));
    connect(ui->gpssend_path,SIGNAL(clicked()),this,SLOT(sendPath()));
    connect(ui->gpssend_point,SIGNAL(clicked()),this,SLOT(sendCoordinate()));
    connect(ui->gpscomboBox_city,SIGNAL(currentIndexChanged(int)),this,SLOT(handleComboSelectionP(int)));
    connect(ui->gpsButton_load2,SIGNAL(clicked()),this,SLOT(loadPlanning()));
    connect(ui->gpsButton_save2,SIGNAL(clicked()),this,SLOT(savePlanning()));
}

void GPSPlanning::setJsObject(){
   // Makes this object available under name from within the frame's JavaScript context
    this->ui->gpswebView3->page()->mainFrame()->addToJavaScriptWindowObject("gpscontrol",this);
}

void GPSPlanning::evalJs(const QString &jsfunction)
{
  //evaluates the javascript of the related html
  ui->gpswebView3->page()->mainFrame()->evaluateJavaScript(jsfunction);
}

void GPSPlanning::controlAdress(qreal test){
    qDebug()<<test;
}

void GPSPlanning::handleString(){
      ui->gpscomboBox_city->setCurrentIndex(0);
    QRegExp re("\\d*");
    if(re.exactMatch(ui->gpslineEdit->text())){
        qDebug()<<"TRUE";
    }

     evalJs(QString("codeAddress('%1')").arg(ui->gpslineEdit->text()));
      ui->gpslineEdit->clear();
}

void GPSPlanning::handleComboSelectionP(int option){
    if(option==1){
        evalJs(QString("setStart(%1,%2)").arg(52.5075419).arg(13.4251364));
    }
    else if(option==2){
         evalJs(QString("setStart(%1,%2)").arg(48.1549107).arg(11.5418357));
    }
    else if(option==3){
         evalJs(QString("setStart(%1,%2)").arg(49.4360936).arg(11.1011232));
    }
    else if(option==4){
        evalJs(QString("setStart(%1,%2)").arg(50.121212).arg(8.6365638));
    }
    else if(option==5){
        evalJs(QString("setStart(%1,%2)").arg(49.7780731).arg(9.9430286));
    }
    else{

    }


}

void GPSPlanning::copterPos(QList<float>* data){
    latitude= data->at(0);
    longitude= data->at(1);
}

void GPSPlanning::handleQuadropos(){
    evalJs(QString("setStartExact(%1,%2)").arg(latitude).arg(longitude));
}

void GPSPlanning::getMapCoordinates(qreal latitude,qreal longitude)
{
     qDebug()<< QString::number((double)latitude, 'f', 7);
    //collects chosen marker positions (Coordinates)
    GPSCoordinate currentData((double)latitude,longitude);
    coordinatePlanning.append(currentData);
    setSingleCoord();
}

void GPSPlanning::deleteMapCoordinates(qreal latitude,qreal longitude)
{
    for(int i=0;i<coordinatePlanning.length();i++)
    {
        if(((coordinatePlanning[i].getLat())==(double)latitude))
        {
            coordinatePlanning.removeAt(i);
        }
    }
    setAllCoords();
}

void GPSPlanning::showClickedMarker(double latitude, double longitude){
   // qDebug()<<latitude;
    ui->gpslabel_4->setText(QString::number(longitude, 'g', 7));
    ui->gpslabel_5->setText(QString::number(latitude, 'g', 7));
}

void GPSPlanning::clearPlanning(){
    qDebug()<<"clear";
    tracking->clearPlannedPath();
    coordinatePlanning.clear();
    evalJs(QString("clear()"));
    model->clear();
    setCoordsView();
    ui->gpslabel_4->setText(QString::number(0));
    ui->gpslabel_5->setText(QString::number(0));
}

void GPSPlanning::setCoordsView(){
    model= new QStandardItemModel();
    model->setHorizontalHeaderItem(0,new QStandardItem("Longitude"));
    model->setHorizontalHeaderItem(1,new QStandardItem("Latitude"));
    ui->gpstableView_2->setModel(model);
}

void GPSPlanning::setSingleCoord(){
    model->setItem(coordinatePlanning.length()-1,0,new QStandardItem(QString::number(coordinatePlanning[coordinatePlanning.length()-1].getLat(), 'g', 7)));
    model->setItem(coordinatePlanning.length()-1,1,new QStandardItem(QString::number(coordinatePlanning[coordinatePlanning.length()-1].getLng(), 'g', 7)));
}

void GPSPlanning::setAllCoords(){
    model->clear();
    setCoordsView();
    for(int i =0; i< coordinatePlanning.length();i++){
    model->setItem(i,0,new QStandardItem(QString::number(coordinatePlanning[i].getLat(), 'g', 7)));
    model->setItem(i,1,new QStandardItem(QString::number(coordinatePlanning[i].getLng(), 'g', 7)));
    }
}

void GPSPlanning::sendPath(){
    tracking->clearPlannedPath();
        for(int i=0;i<coordinatePlanning.length();i++)
        {
        Frame f;
        f.addMessage(GPS_WAYPOINTLIST);
        f.addValue(i);
        f.addValue((float)coordinatePlanning[i].getLng());
        f.addValue((float)coordinatePlanning[i].getLat());
        f.endAndSendFrame(serial);
        }
        Frame f2;
        f2.addMessage(GPS_WAYPOINTLIST);
        f2.addValue(-1);
        f2.addValue( ui->gpslineEdit_1->text().toFloat());
        f2.endAndSendFrame(serial);

        sollList.clear();
        ui->gpssend_point->setEnabled(false);
        ui->gpssend_path->setEnabled(false);
        tracking->showPlannedPath(coordinatePlanning);
}

void GPSPlanning::sendCoordinate(){
    Frame f;
    f.addMessage(GPS_SINGLE_WAYPOINTS);
    tracking->showPlannedCoord(coordinatePlanning[0]);
    f.addValue(coordinatePlanning[0].getLng());
    f.addValue(coordinatePlanning[0].getLat());
    f.endAndSendFrame(serial);
    coordinatePlanning.removeAt(0);
    ui->gpssend_point->setEnabled(false);
    ui->gpssend_path->setEnabled(false);

}

void GPSPlanning::receiveAck(float data){
    if(data==1){
        ui->gpssend_point->setEnabled(true);
        ui->gpssend_path->setEnabled(true);
    }
}

void GPSPlanning::savePlanning(){
        QDateTime dateTimeSys = QDateTime::currentDateTime();
        dateTimeSysString=dateTimeSys.toString("ddd dd.MMM.yyyy hh:mm");
QString filename = QFileDialog::getSaveFileName(0,QString(),QString(),"CSV Files (*.csv)");
if (!filename.isNull() && !filename.isEmpty())
{
        // Open file
        QFile save(filename);
        if (!save.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        {
                this->ui->console->append(QString("Could not open ").append(filename.trimmed()).append(" for writing.\n"));
                return;
        }
        QTextStream out(&save);
        out.setLocale(QLocale::system());
        out<<";"<<"GPS Planning List"<<"\n";
        out<<"Date of Planning:"<<dateTimeSysString<<"\n";
        out<<"Latitude"<<";"<<"Longitude"<<";"<<"Height"<<"\n";
        // Write config
        for(int i=0;i<coordinatePlanning.length();i++)
        {   GPSCoordinate planning = coordinatePlanning.at(i);
            out<<QString::number(planning.getLat(), 'g', 7)<<";"<<QString::number(planning.getLng(), 'g', 7)<<";"<<QString::number(planning.getHght(), 'g', 7)<<"\n";
        }
        out.flush();
        save.close();
    }
}
void GPSPlanning::loadPlanning(){
       clearPlanning();

       QString filename = QFileDialog::getOpenFileName(this,
                                       QString(),
                                      QString(),
                                       tr("Config file (*.csv)"));
       if (!filename.isNull() && !filename.isEmpty()) {
               // Open file
               QFile load(filename);
               if (!load.open(QIODevice::ReadOnly | QIODevice::Text)) {
                       this->ui->console->append(QString("Could not open ").append(filename.trimmed()).append(" for reading.\n"));
                       return;
               }

               QTextStream in(&load);
               in.setLocale(QLocale::system());
               QString line;
               int i=0;
               while (!in.atEnd()) {
                       line = in.readLine();
                       if(i>2){
                       QStringList items=line.split(";",QString::SkipEmptyParts);
                       currentData.setAll(items[0].toDouble(),items[1].toDouble(),items[2].toDouble());
                       coordinatePlanning.append(currentData);
                       evalJs(QString("setPath(%1,%2)").arg(QString::number(currentData.getLat(), 'g', 7)).arg(QString::number(currentData.getLng(), 'g', 7)));
                   }
                       i++;
                   }
               setAllCoords();
           }
}

void GPSPlanning::loadDialog(){
    QMessageBox msgBox;
     msgBox.setText("Are you sure you want to delete all further data? ");
     msgBox.addButton(QMessageBox::Yes);
     msgBox.addButton(QMessageBox::No);
     msgBox.addButton(QMessageBox::Save);


       int selection = msgBox.exec();
       if(selection == QMessageBox::Yes){
           loadPlanning();
       }
       else if(selection == QMessageBox::No)
       {
       }
       else if(selection == QMessageBox::Save){
           savePlanning();
           loadPlanning();
       }

}
GPSPlanning::~GPSPlanning()
{

}
