#include "gpscoordinate.h"


GPSCoordinate::GPSCoordinate()
{
     latitude=0;longitude=0;height=0;
}

GPSCoordinate::GPSCoordinate(double ilatitude, double ilongitude): height(-1.0){
        latitude=ilatitude; longitude=ilongitude;
}

GPSCoordinate::GPSCoordinate(double ilatitude, double ilongitude, double iheight){
        latitude=ilatitude; longitude=ilongitude;height=iheight;
}
double GPSCoordinate::getLat()
{
    return latitude;
}
double GPSCoordinate::getLng()
{
    return longitude;
}
double GPSCoordinate::getHght()
{
    return height;
}
void GPSCoordinate::setLat(double ilatitude)
{
    latitude=ilatitude;
}
void GPSCoordinate::setLng(double ilongitude)
{
    longitude=ilongitude;
}
void GPSCoordinate::setHght(double iheight)
{
    height=iheight;
}
void GPSCoordinate::setAll(double ilatitude, double ilongitude,double iheight)
{
    this->latitude=ilatitude; this->longitude=ilongitude;this->height=iheight;
}

bool GPSCoordinate::compareCoords(double ilatitude, double ilongitude)
{
    //((round(latitude)==round(ilatitude))&&(round(longitude)==round(ilongitude)));
//    qDebug()<<round(latitude);
//    qDebug()<<round(ilatitude);
//    qDebug()<<round(longitude);
//    qDebug()<<round(ilongitude);
    return ((((latitude==ilatitude)&&(longitude==ilongitude))));
}

bool GPSCoordinate::round(double a)
{
    int tmp=a*10*10;
    return tmp/(10*10);
}
