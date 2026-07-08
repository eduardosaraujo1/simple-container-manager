#ifndef CONTAINERTABLEMODEL_H
#define CONTAINERTABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include "data-objects/managedcontainer.h"

class ContainerTableModel : public QAbstractTableModel
{
public:
    explicit ContainerTableModel(QObject *parent = nullptr);

private:
    QList<ManagedContainer> m_containers;
};

#endif // CONTAINERTABLEMODEL_H
