#include "interfacedialog.h"
#include "ui_interfacedialog.h"

#include "QLabel"
#include "QDebug"
#include "QSerialPort"
#include <QtSerialPort/QSerialPortInfo>

InterfaceDialog::InterfaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InterfaceDialog)
{
    ui->setupUi(this);
    layout = new QGridLayout;
    ui->groupBox_connectionSettings->setLayout(layout);

    upperLabel = new QLabel("Port");
    middleLabel = new QLabel("Baudrate");
    lowerLabel = new QLabel("");

    com = new QComboBox;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
#ifdef Q_OS_LINUX
        com->addItem("/dev/" + info.portName());
#else
        com->addItem(info.portName());
#endif
    }
    baudrate = new QComboBox;
    baudrate->addItem("9600");
    baudrate->addItem("19200");
    baudrate->addItem("38400");
    baudrate->addItem("57600");
    baudrate->addItem("115200");

    layout->addWidget(upperLabel,0,0,1,1);
    layout->addWidget(middleLabel,1,0,1,1);
    layout->addWidget(lowerLabel,2,0,1,1);
    layout->addWidget(com,0,1,1,1);
    layout->addWidget(baudrate,1,1,1,1);

    host = new QLineEdit;
    host->setText("10.0.3.8");
    host->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    antenna = new QLineEdit;
    antenna->setText("10.0.3.34");
    antenna->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    port = new QLineEdit;
    port->setText("5003");
    port->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    active = 0;
}

InterfaceDialog::~InterfaceDialog()
{
    delete ui;
}


void InterfaceDialog::on_pushButton_connect_clicked()
{
    if(ui->pushButton_connect->text().compare("Connect") == 0){
        QStringList list;
        if(active == 0){
            list.append(com->currentText());
            list.append(baudrate->currentText());
            baudrate->setEnabled(false);
            com->setEnabled(false);

            emit initConnection(true, list);
        } else {
            list.append(host->text());
            list.append(antenna->text());
            list.append(port->text());
            host->setEnabled(false);
            antenna->setEnabled(false);
            port->setEnabled(false);
            emit initConnection(false, list);
        }
        ui->comboBox->setEnabled(false);
        ui->pushButton_connect->setText("Disconnect");
    } else {
        if(active == 0){
            baudrate->setEnabled(true);
            com->setEnabled(true);
            emit disconnect(true);
        } else {
            host->setEnabled(true);
            antenna->setEnabled(true);
            port->setEnabled(true);
            emit disconnect(false);
        }
        ui->comboBox->setEnabled(true);
        ui->pushButton_connect->setText("Connect");
    }
}

void InterfaceDialog::on_comboBox_currentIndexChanged(int index)
{

    if(index == 0){
        if(active != 0){
            antenna->setParent(NULL);
            host->setParent(NULL);
            port->setParent(NULL);
        }
        upperLabel->setText("Port");
        middleLabel->setText("Baudrate");
        lowerLabel->setText("");
        layout->addWidget(com,0,1,1,1);
        layout->addWidget(baudrate,1,1,1,1);

    } else if (index == 1){
        if(active != 1){
            com->setParent(NULL);
            baudrate->setParent(NULL);
        }
        upperLabel->setText("Antenna IP");
        middleLabel->setText("Local IP");
        lowerLabel->setText("Port");
        layout->addWidget(host,0,1,1,1);
        layout->addWidget(antenna,1,1,1,1);
        layout->addWidget(port,2,1,1,1);
    }

    active = index;
}
