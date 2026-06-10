#include "containerinfo.h"
#include "containerstatus.h"
#include <QJsonObject>

ContainerInfo ContainerInfo::fromJsonObject(const QJsonObject &obj, bool &error) {
    error = false;
    if (! (obj.contains("id")
          && obj.contains("name")
          && obj.contains("status")
          )) {
        error = true;
        return ContainerInfo{};
    }
    ContainerStatus status = containerStatusFromString(obj.value("status").toString());

    return ContainerInfo{
        obj.value("id").toString(),
        obj.value("name").toString(),
        status
    };
}
