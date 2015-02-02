/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "copter.h"

#include <QGLWidget>
#include <QMatrix4x4>
#include <QVector3D>

#include <qmath.h>

static const qreal tee_height = 0.15; //0.311126;
static const qreal cross_width = 0.025;
static const qreal bar_thickness = 0.0113137;
static const qreal logo_depth = 0.010;

//! [0]
struct Geometry
{
    QVector<GLushort> faces;
    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    void appendSmooth(const QVector3D &a, const QVector3D &n, int from);
    void appendFaceted(const QVector3D &a, const QVector3D &n);
    void finalize();
    void loadArrays() const;
};
//! [0]

//! [1]
class Patch
{
public:
    enum Smoothing { Faceted, Smooth };
    Patch(Geometry *);
    void setSmoothing(Smoothing s) { sm = s; }
    void translate(const QVector3D &t);
    void rotate(qreal deg, QVector3D axis);
    void draw() const;
    void addTri(const QVector3D &a, const QVector3D &b, const QVector3D &c, const QVector3D &n);
    void addQuad(const QVector3D &a, const QVector3D &b,  const QVector3D &c, const QVector3D &d);

    GLushort start;
    GLushort count;
    GLushort initv;

    GLfloat faceColor[4];
    QMatrix4x4 mat;
    Smoothing sm;
    Geometry *geom;
};
//! [1]

static inline void qSetColor(float colorVec[], QColor c)
{
    colorVec[0] = c.redF();
    colorVec[1] = c.greenF();
    colorVec[2] = c.blueF();
    colorVec[3] = c.alphaF();
}

void Geometry::loadArrays() const
{
    glVertexPointer(3, GL_FLOAT, 0, vertices.constData());
    glNormalPointer(GL_FLOAT, 0, normals.constData());
}

void Geometry::finalize()
{
    // TODO: add vertex buffer uploading here

    // Finish smoothing normals by ensuring accumulated normals are returned
    // to length 1.0.
    for (int i = 0; i < normals.count(); ++i)
        normals[i].normalize();
}

void Geometry::appendSmooth(const QVector3D &a, const QVector3D &n, int from)
{
    // Smooth normals are achieved by averaging the normals for faces meeting
    // at a point.  First find the point in geometry already generated
    // (working backwards, since most often the points shared are between faces
    // recently added).
    int v = vertices.count() - 1;
    for ( ; v >= from; --v)
        if (qFuzzyCompare(vertices[v], a))
            break;

    if (v < from) {
        // The vertex was not found so add it as a new one, and initialize
        // its corresponding normal
        v = vertices.count();
        vertices.append(a);
        normals.append(n);
    } else {
        // Vert found, accumulate normals into corresponding normal slot.
        // Must call finalize once finished accumulating normals
        normals[v] += n;
    }

    // In both cases (found or not) reference the vertex via its index
    faces.append(v);
}

void Geometry::appendFaceted(const QVector3D &a, const QVector3D &n)
{
    // Faceted normals are achieved by duplicating the vertex for every
    // normal, so that faces meeting at a vertex get a sharp edge.
    int v = vertices.count();
    vertices.append(a);
    normals.append(n);
    faces.append(v);
}

Patch::Patch(Geometry *g)
    : start(g->faces.count())
    , count(0)
    , initv(g->vertices.count())
    , sm(Patch::Smooth)
    , geom(g)
{
    qSetColor(faceColor, QColor(Qt::white)); // Qt::darkGray
}

void Patch::rotate(qreal deg, QVector3D axis)
{
    mat.rotate(deg, axis);
}

void Patch::translate(const QVector3D &t)
{
    mat.translate(t);
}

//! [2]
void Patch::draw() const
{
    glPushMatrix();
    glMultMatrixf(mat.constData());
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, faceColor);

    const GLushort *indices = geom->faces.constData();
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, indices + start);
    glPopMatrix();
}
//! [2]

void Patch::addTri(const QVector3D &a, const QVector3D &b, const QVector3D &c, const QVector3D &n)
{
    QVector3D norm = n.isNull() ? QVector3D::normal(a, b, c) : n;

    if (sm == Smooth) {
        geom->appendSmooth(a, norm, initv);
        geom->appendSmooth(b, norm, initv);
        geom->appendSmooth(c, norm, initv);
    } else {
        geom->appendFaceted(a, norm);
        geom->appendFaceted(b, norm);
        geom->appendFaceted(c, norm);
    }

    count += 3;
}

void Patch::addQuad(const QVector3D &a, const QVector3D &b,  const QVector3D &c, const QVector3D &d)
{
    QVector3D norm = QVector3D::normal(a, b, c);

    if (sm == Smooth) {
        addTri(a, b, c, norm);
        addTri(a, c, d, norm);
    } else {
        // If faceted share the two common vertices
        addTri(a, b, c, norm);
        int k = geom->vertices.count();
        geom->appendSmooth(a, norm, k);
        geom->appendSmooth(c, norm, k);
        geom->appendFaceted(d, norm);
        count += 3;
    }
}

static inline QVector<QVector3D> extrude(const QVector<QVector3D> &vertices, qreal depth)
{
    QVector<QVector3D> extr = vertices;
    for (int v = 0; v < extr.count(); ++v)
        extr[v].setZ(extr[v].z() - depth);
    return extr;
}

class Rectoid
{
public:
    void translate(const QVector3D &t)
    {
        for (int i = 0; i < parts.count(); ++i)
            parts[i]->translate(t);
    }

    void rotate(qreal deg, QVector3D axis)
    {
        for (int i = 0; i < parts.count(); ++i)
            parts[i]->rotate(deg, axis);
    }

    // No special Rectoid destructor - the parts are fetched out of this member
    // variable, and destroyed by the new owner
    QList<Patch *> parts;
};

class RectPrism : public Rectoid
{
public:
    RectPrism(Geometry *g, qreal width, qreal height, qreal depth);
};

RectPrism::RectPrism(Geometry *g, qreal width, qreal height, qreal depth)
{
    enum { bl, br, tr, tl };
    Patch *fb = new Patch(g);
    fb->setSmoothing(Patch::Faceted);

    // front face
    QVector<QVector3D> r(4);
    r[br].setX(width);
    r[tr].setX(width);
    r[tr].setY(height);
    r[tl].setY(height);
    QVector3D adjToCenter(-width / 2.0, -height / 2.0, depth / 2.0);
    for (int i = 0; i < 4; ++i)
        r[i] += adjToCenter;
    fb->addQuad(r[bl], r[br], r[tr], r[tl]);

    // back face
    QVector<QVector3D> s = extrude(r, depth);
    fb->addQuad(s[tl], s[tr], s[br], s[bl]);

    // side faces
    Patch *sides = new Patch(g);
    sides->setSmoothing(Patch::Faceted);
    sides->addQuad(s[bl], s[br], r[br], r[bl]);
    sides->addQuad(s[br], s[tr], r[tr], r[br]);
    sides->addQuad(s[tr], s[tl], r[tl], r[tr]);
    sides->addQuad(s[tl], s[bl], r[bl], r[tl]);

    parts << fb << sides;
}

class RectTorus : public Rectoid
{
public:
    RectTorus(Geometry *g, qreal iRad, qreal oRad, qreal depth, int numSectors);
};

RectTorus::RectTorus(Geometry *g, qreal iRad, qreal oRad, qreal depth, int k)
{
    QVector<QVector3D> inside;
    QVector<QVector3D> outside;
    for (int i = 0; i < k; ++i) {
        qreal angle = (i * 2 * M_PI) / k;
        inside << QVector3D(iRad * qSin(angle), iRad * qCos(angle), depth / 2.0);
        outside << QVector3D(oRad * qSin(angle), oRad * qCos(angle), depth / 2.0);
    }
    inside << QVector3D(0.0, iRad, 0.0);
    outside << QVector3D(0.0, oRad, 0.0);
    QVector<QVector3D> in_back = extrude(inside, depth);
    QVector<QVector3D> out_back = extrude(outside, depth);

    // Create front, back and sides as separate patches so that smooth normals
    // are generated for the curving sides, but a faceted edge is created between
    // sides and front/back
    Patch *front = new Patch(g);
    for (int i = 0; i < k; ++i)
        front->addQuad(outside[i], inside[i],
                       inside[(i + 1) % k], outside[(i + 1) % k]);
    Patch *back = new Patch(g);
    for (int i = 0; i < k; ++i)
        back->addQuad(in_back[i], out_back[i],
                      out_back[(i + 1) % k], in_back[(i + 1) % k]);
    Patch *is = new Patch(g);
    for (int i = 0; i < k; ++i)
        is->addQuad(in_back[i], in_back[(i + 1) % k],
                inside[(i + 1) % k], inside[i]);
    Patch *os = new Patch(g);
    for (int i = 0; i < k; ++i)
        os->addQuad(out_back[(i + 1) % k], out_back[i],
                outside[i], outside[(i + 1) % k]);
    parts << front << back << is << os;
}

Copter::Copter(QObject *parent, int divisions, qreal scale)
    : QObject(parent)
    , geom(new Geometry())
{
    buildGeometry(divisions, scale);
}

Copter::~Copter()
{
    qDeleteAll(parts);
    delete geom;
}

void Copter::setColor(QColor c)
{

}

//! [3]
void Copter::buildGeometry(int divisions, qreal scale)
{

    QVector3D y(0.0, 1.0, 0.0);
    QVector3D x(1.0, 0.0, 0.0);

//    RectPrism armv(geom, 0.02999, 0.02999, 0.35);
//    armv.translate(QVector3D(0,0,-0.175));
//    RectPrism armb(geom, 0.02999, 0.02999, 0.35);
//    armb.translate(QVector3D(0,0,0.175));
//    RectPrism arm2(geom, 0.03, 0.03, 0.7);
//    arm2.rotate(60, y);
//    RectPrism arm3(geom, 0.03, 0.03, 0.7);
//    arm3.rotate(120, y);

//    RectTorus rot1(geom, 0.09, 0.12, 0.03, divisions);
//    rot1.translate(QVector3D(0,0,-0.35));
//    rot1.rotate(90, x);
//    RectTorus rot2(geom, 0.09, 0.12, 0.03, divisions);
//    rot2.translate(QVector3D(0.,0,0.35));
//    rot2.rotate(90, x);
//    RectTorus rot3(geom, 0.09, 0.12, 0.03, divisions);
//    rot3.translate(QVector3D(cos(30./180*M_PI)*0.35,0,sin(30./180*M_PI)*0.35));
//    rot3.rotate(90, x);
//    RectTorus rot4(geom, 0.09, 0.12, 0.03, divisions);
//    rot4.translate(QVector3D(-cos(30./180*M_PI)*0.35,0,-sin(30./180*M_PI)*0.35));
//    rot4.rotate(90, x);
//    RectTorus rot5(geom, 0.09, 0.12, 0.03, divisions);
//    rot5.translate(QVector3D(-cos(30./180*M_PI)*0.35,0,sin(30./180*M_PI)*0.35));
//    rot5.rotate(90, x);
//    RectTorus rot6(geom, 0.09, 0.12, 0.03, divisions);
//    rot6.translate(QVector3D(cos(30./180*M_PI)*0.350,0,-sin(30./180*M_PI)*0.35));
//    rot6.rotate(90, x);

//    QColor green = QColor::fromRgb(0,200,0);
//    QColor red = QColor::fromRgb(200,0,0);
//    QColor blue = QColor::fromRgb(0,0,200);

//    parts << arm2.parts << arm3.parts << rot2.parts << rot3.parts << rot4.parts << rot5.parts << rot6.parts;

//    for (int i = 0; i < parts.count(); ++i)
//        qSetColor(parts[i]->faceColor, green);
//    int last = parts.count();
//    parts << armv.parts << rot1.parts;
//    for (int i = last; i < parts.count(); ++i)
//        qSetColor(parts[i]->faceColor, red);
//    last = parts.count();
//    parts << armb.parts << rot2.parts;
//    for (int i = last; i < parts.count(); ++i)
//        qSetColor(parts[i]->faceColor, blue);

    RectPrism arm1(geom, 0.03, 0.03, 0.3);
    arm1.translate(QVector3D(0,0,-0.175));

    RectPrism arm2(geom, 0.03, 0.03, 0.3);
    arm2.translate(QVector3D(-sin(M_PI / 3) * 0.175,0,-cos(M_PI / 3) * 0.175));
    arm2.rotate(60, y);

    RectPrism arm3(geom, 0.03, 0.03, 0.3);
    arm3.translate(QVector3D(sin(M_PI / 3) * 0.175,0,cos(M_PI / 3) * 0.175));
    arm3.rotate(60, y);

    RectPrism arm4(geom, 0.03, 0.03, 0.3);
    arm4.translate(QVector3D(0,0,0.175));

    RectPrism arm5(geom, 0.03, 0.03, 0.3);
    arm5.translate(QVector3D(-sin(M_PI / 3) * 0.175,0,cos(M_PI / 3) * 0.175));
    arm5.rotate(-60, y);

    RectPrism arm6(geom, 0.03, 0.03, 0.3);
    arm6.translate(QVector3D(sin(M_PI / 3) * 0.175,0,-cos(M_PI / 3) * 0.175));
    arm6.rotate(-60, y);


    RectTorus rot1(geom, 0.09, 0.12, 0.03, divisions);
    rot1.translate(QVector3D(0,0,-0.35));
    rot1.rotate(90, x);
    RectTorus rot2(geom, 0.09, 0.12, 0.03, divisions);
    rot2.translate(QVector3D(0.,0,0.35));
    rot2.rotate(90, x);
    RectTorus rot3(geom, 0.09, 0.12, 0.03, divisions);
    rot3.translate(QVector3D(cos(30./180*M_PI)*0.35,0,sin(30./180*M_PI)*0.35));
    rot3.rotate(90, x);
    RectTorus rot4(geom, 0.09, 0.12, 0.03, divisions);
    rot4.translate(QVector3D(-cos(30./180*M_PI)*0.35,0,-sin(30./180*M_PI)*0.35));
    rot4.rotate(90, x);
    RectTorus rot5(geom, 0.09, 0.12, 0.03, divisions);
    rot5.translate(QVector3D(-cos(30./180*M_PI)*0.35,0,sin(30./180*M_PI)*0.35));
    rot5.rotate(90, x);
    RectTorus rot6(geom, 0.09, 0.12, 0.03, divisions);
    rot6.translate(QVector3D(cos(30./180*M_PI)*0.350,0,-sin(30./180*M_PI)*0.35));
    rot6.rotate(90, x);


    QColor gray = QColor::fromRgb(145,145,145);
    QColor green = QColor::fromRgb(34,139,34);
    QColor orange = QColor::fromRgb(255,140,0);

    parts << arm2.parts << arm4.parts << arm6.parts << rot2.parts << rot4.parts << rot6.parts;

    for (int i = 0; i < parts.count(); ++i)
        qSetColor(parts[i]->faceColor, gray);
    int last = parts.count();
    parts << arm1.parts << rot1.parts;
    for (int i = last; i < parts.count(); ++i)
        qSetColor(parts[i]->faceColor, orange);
    last = parts.count();
    parts << arm3.parts << arm5.parts << rot3.parts << rot5.parts ;
    for (int i = last; i < parts.count(); ++i)
        qSetColor(parts[i]->faceColor, green);

    geom->finalize();
}
//! [3]

//! [4]
void Copter::draw() const
{
    geom->loadArrays();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    for (int i = 0; i < parts.count(); ++i)
        parts[i]->draw();

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}
//! [4]
