#include "myitem.h"

TopicItem::TopicItem() :
    QListWidgetItem()
{
}

void TopicItem::addField(PlotItem *field)
{
    fields.push_back(field);
}

void TopicItem::setFieldsHidden(bool hidden)
{
    foreach(QListWidgetItem* i, fields){
        i->setHidden(hidden);
    }
}


PlotItem::PlotItem() :
    QListWidgetItem()
{
}

void PlotItem::setGraphID(int id)
{
    graphID = id;
}


