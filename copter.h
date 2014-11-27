#ifndef COPTER_H
#define COPTER_H

#include <QObject>
#include <QColor>

class Patch;
struct Geometry;

class Copter : public QObject
{
public:
    explicit Copter(QObject *parent, int d = 64, qreal s = 1.0);
    ~Copter();
    void setColor(QColor c);
    void draw() const;
private:
    void buildGeometry(int d, qreal s);

    QList<Patch *> parts;
    Geometry *geom;
};

#endif // COPTER_H
