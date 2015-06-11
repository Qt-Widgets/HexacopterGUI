#include "worldmagneticmodel.h"
#include "magnetic/EGM9615.h"
#include <stdlib.h>

#include <QDebug>
#include <QDate>

char filename[] = "WMM.COF";

WorldMagneticModel::WorldMagneticModel()
{
    int NumTerms, Flag = 1, nMax = 0;
    int epochs = 1;



    if(!MAG_robustReadMagModels(filename,(MAGtype_MagneticModel***) &MagneticModels, epochs)) {
        qDebug() << "Could not open WMM Configuration file!";
    }
    if(nMax < MagneticModels[0]->nMax) nMax = MagneticModels[0]->nMax;
    NumTerms = ((nMax + 1) * (nMax + 2) / 2);
    TimedMagneticModel = MAG_AllocateModelMemory(NumTerms); /* For storing the time modified WMM Model parameters */
    if(MagneticModels[0] == NULL || TimedMagneticModel == NULL)
    {
        MAG_Error(2);
    }
    MAG_SetDefaults(&Ellip, &Geoid); /* Set default values and constants */
    /* Check for Geographic Poles */

    /* Set EGM96 Geoid parameters */
    Geoid.GeoidHeightBuffer = EGM9615;
    Geoid.Geoid_Initialized = 1;

    qDebug() << "still alive";
}

WorldMagneticModel::~WorldMagneticModel() {
    MAG_FreeMagneticModelMemory(TimedMagneticModel);
    MAG_FreeMagneticModelMemory(MagneticModels[0]);
}

double WorldMagneticModel::getMagDeclination(double lat, double lon, double seaLevel)
{
    CoordGeodetic.lambda = lon;
    CoordGeodetic.phi = lat;
    CoordGeodetic.HeightAboveGeoid = seaLevel;


    Geoid.UseGeoid = 1;
    CoordGeodetic.UseGeoid = Geoid.UseGeoid;
    MAG_ConvertGeoidToEllipsoidHeight(&CoordGeodetic, &Geoid);


    QDate date = QDate::currentDate();
    UserDate.Day = date.day();
    UserDate.Month = date.month();
    UserDate.Year = date.year();


    char *Error_Message;
    MAG_DateToYear(&UserDate, Error_Message);

    MAG_GeodeticToSpherical(Ellip, CoordGeodetic, &CoordSpherical); /*Convert from geodetic to Spherical Equations: 17-18, WMM Technical report*/
    MAG_TimelyModifyMagneticModel(UserDate, MagneticModels[0], TimedMagneticModel); /* Time adjust the coefficients, Equation 19, WMM Technical report */
    MAG_Geomag(Ellip, CoordSpherical, CoordGeodetic, TimedMagneticModel, &GeoMagneticElements); /* Computes the geoMagnetic field elements and their time change*/
    MAG_CalculateGridVariation(CoordGeodetic, &GeoMagneticElements);
    MAG_WMMErrorCalc(GeoMagneticElements.H, &Errors);

    return GeoMagneticElements.Decl;
}
