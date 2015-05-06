#ifndef GPSCOORDINATE_H
#define GPSCOORDINATE_H
#include <QtCore/QtGlobal>
#include "ui_mainwindow.h"

class GPSCoordinate{

public:
    double latitude;
    double longitude;
    double height;
    GPSCoordinate();
    GPSCoordinate(double latitude, double longitude);
    GPSCoordinate(double latitude, double longitude, double height);
    double getLat();
    double getLng();
    double getHght();
    void setLat(double ilatitude);
    void setLng(double ilongitude);
    void setHght(double iheight);
    void setAll(double ilatitude, double ilongitude,double iheight);
    bool compareCoords(double ilatitude, double ilongitude);
private:
    bool round(double a);
};

#endif // GPSCOORDINATE_H
