#ifndef CONTAINERINFO_H
#define CONTAINERINFO_H

#include <QJsonObject>
#include "containerstatus.h"

struct ContainerInfo
{
    QString id;
    QString name;
    ContainerStatus status;

    static ContainerInfo fromJsonObject(const QJsonObject &obj, bool &error);
};

#endif // CONTAINERINFO_H
