#ifndef CONTAINERTABLEMODEL_H
#define CONTAINERTABLEMODEL_H

#include <QAbstractTableModel>

class ContainerTableModel : public QAbstractTableModel
{
public:
    explicit ContainerTableModel(QObject *parent = nullptr);
};

#endif // CONTAINERTABLEMODEL_H
