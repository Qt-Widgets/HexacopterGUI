#ifndef CAMERADIALOG_H
#define CAMERADIALOG_H

#include <QDialog>

namespace Ui {
class cameradialog;
}

class cameradialog : public QDialog
{
    Q_OBJECT
    QString device;

public:
    explicit cameradialog(QWidget *parent = 0);
    ~cameradialog();

private:
    Ui::cameradialog *ui;

signals:
    void openDev(int i);
    void closeDev();
private slots:
    void on_pushButton_disconnect_clicked();
    void on_pushButton_connect_clicked();
};

#endif // CAMERADIALOG_H
