#ifndef WORLDMAGNETICMODEL_H
#define WORLDMAGNETICMODEL_H

#include "magnetic/Geomagnetism.h"

class WorldMagneticModel{
    MAGtype_MagneticModel * MagneticModels[1], *TimedMagneticModel;
    MAGtype_Ellipsoid Ellip;
    MAGtype_CoordSpherical CoordSpherical;
    MAGtype_CoordGeodetic CoordGeodetic;
    MAGtype_Date UserDate;
    MAGtype_GeoMagneticElements GeoMagneticElements, Errors;
    MAGtype_Geoid Geoid;

public:
    WorldMagneticModel();
    ~WorldMagneticModel();
    double getMagDeclination(double lat, double lon, double seaLevel);

};

#endif // WORLDMAGNETICMODEL_H
