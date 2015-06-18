#include "cameradialog.h"
#include "ui_cameradialog.h"
#include <QDebug>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int testDevIdx(QString dev){
    int fd;
    if((fd = open(dev.toLatin1(), O_RDONLY)) == -1){
        return 0;
    }
    close(fd);
    return 1;
}

cameradialog::cameradialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::cameradialog)
{
    ui->setupUi(this);

    int i = 0;
    QString dev = "/dev/video" + QString::number(i);
    while(testDevIdx(dev)){
        ui->comboBox_videoDev->addItem(dev);
        i++;
        dev = "/dev/video" + QString::number(i);
    }
}

cameradialog::~cameradialog()
{
    delete ui;
}

void cameradialog::on_pushButton_disconnect_clicked()
{
    ui->pushButton_connect->setEnabled(true);
    ui->comboBox_videoDev->setEnabled(true);
    ui->pushButton_disconnect->setEnabled(false);
    emit closeDev();
}

void cameradialog::on_pushButton_connect_clicked()
{
    ui->pushButton_connect->setEnabled(false);
    ui->comboBox_videoDev->setEnabled(false);
    ui->pushButton_disconnect->setEnabled(true);
    emit openDev(ui->comboBox_videoDev->currentIndex());
}
