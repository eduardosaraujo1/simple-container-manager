#ifndef DOCKEREVENTSTREAM_H
#define DOCKEREVENTSTREAM_H

#include <QObject>
#include <model/data-objects/dockerevent.h>

class DockerEventStream : public QObject
{
    Q_OBJECT
public:
    explicit DockerEventStream(QObject *parent = nullptr);
signals:
    void eventReceived(const DockerEvent event);
};

#endif // DOCKEREVENTSTREAM_H
