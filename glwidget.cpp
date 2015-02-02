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

#include <QtWidgets>
#include <QtOpenGL>

#include <math.h>

#include "glwidget.h"
#include "copter.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif


GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    copter = 0;
    xRot = 0;
    yRot = 0;
    zRot = 0;

    black = QColor::fromCmykF(0.0, 0.0, 0.0, 1.0);
}

GLWidget::~GLWidget()
{
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 350);
}


static void qNormalizeAngle(float &angle)
{
    while (angle < 0)
        angle += 360;
    while (angle > 360)
        angle -= 360;
}

void GLWidget::setXRotation(float angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        updateGL();
    }
}

void GLWidget::setYRotation(float angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        updateGL();
    }
}

void GLWidget::setZRotation(float angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        updateGL();
    }
}

void GLWidget::setEuler(float roll, float pitch, float yaw)
{
    qNormalizeAngle(roll);
    if (roll != xRot) {
        xRot = roll;
    }

    qNormalizeAngle(pitch);
    if (pitch != yRot) {
        yRot = pitch;
    }
    qNormalizeAngle(yaw);
    if (yaw != zRot) {
        zRot = yaw;
    }
    updateGL();
}


void GLWidget::initializeGL()
{
    qglClearColor(black.dark());

    copter = new Copter(this, 64);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
    static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    glRotatef(zRot, 0.0, -1.0, 0.0);
    glRotatef(yRot, 1.0, 0.0, 0.0);
    glRotatef(xRot, 0.0, 0.0, -1.0);
//    glRotatef(zRot, 0.0, 1.0, 0.0);
//    glRotatef(yRot, -1.0, 0.0, 0.0);
//    glRotatef(xRot, 0.0, 0.0, -1.0);
    copter->draw();
}

void GLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
#else
    glOrtho(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + dy);
        setYRotation(yRot + dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + dy);
        setZRotation(zRot + dx);
    }
    lastPos = event->pos();
}

