#ifndef CONTAINERSERVICE_H
#define CONTAINERSERVICE_H

#include <QObject>

class ContainerService : public QObject {
    Q_OBJECT
public:
    explicit ContainerService(QObject* parent = nullptr);
}

#endif // CONTAINERSERVICE_H