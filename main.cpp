#include <QDebug>
#include "mainwindow.h"
#include <QApplication>

#include "worldmagneticmodel.h"

int main(int argc, char *argv[]){

    QApplication a(argc, argv);
    setlocale(LC_NUMERIC,"C");

    WorldMagneticModel wmm;

//    qDebug() << wmm.getMagDeclination(49.785423, 9.974491,0);

    MainWindow w;
    w.showMaximized();
    w.show();
    return a.exec();
}

