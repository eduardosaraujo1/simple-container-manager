#include "containertablemodel.h"

ContainerTableModel::ContainerTableModel(QObject *parent)
    : QAbstractTableModel{parent}
{}

// containertablemodel.cpp:8:26: Out-of-line definition of 'rowCount' does not match any declaration in 'ContainerTableModel'
// containertablemodel.h:8:7: ContainerTableModel defined here
int ContainerTableModel::rowCount(const QModelIndex &parent=QModelIndex()) const {
    return m_containers.count();
}

int ContainerTableModel::columnCount(const QModelIndex &parent=QModelIndex()) const {
    //
    return 4;
}