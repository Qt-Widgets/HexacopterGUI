#ifndef MYITEM_H
#define MYITEM_H

#include <QListWidgetItem>

class PlotItem : public QListWidgetItem
{

public:
    explicit PlotItem();
    void setGraphID(int id);
    int graphID;

signals:

public slots:

};

class TopicItem : public QListWidgetItem
{

public:
    explicit TopicItem();
    void addField(PlotItem* field);
    void setFieldsHidden(bool hidden);
    int size(){return fields.size();}
    QList<PlotItem*> fields;

signals:

public slots:

};



#endif // MYITEM_H
