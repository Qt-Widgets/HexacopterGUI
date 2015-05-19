#ifndef INTERFACEDIALOG_H
#define INTERFACEDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QGridLayout>


namespace Ui {
class InterfaceDialog;
}

class InterfaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InterfaceDialog(QWidget *parent = 0);
    ~InterfaceDialog();



private slots:
    void on_pushButton_connect_clicked();
    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::InterfaceDialog *ui;

    QGridLayout *layout;

    QLabel *upperLabel;
    QLabel *middleLabel;
    QLabel *lowerLabel;
    QComboBox *baudrate;
    QComboBox *com;
    QLineEdit *host;
    QLineEdit *antenna;
    QLineEdit *port;

    int active;

signals:
    void initConnection(bool serial, QStringList data);
    void disconnect(bool serial);

};

#endif // INTERFACEDIALOG_H
