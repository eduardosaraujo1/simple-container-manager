#ifndef DOCKEREVENTSTREAM_H
#define DOCKEREVENTSTREAM_H

#include <QObject>

class DockerEventStream : public QObject
{
    Q_OBJECT
public:
    explicit DockerEventStream(QObject *parent = nullptr);

signals:
};

#endif // DOCKEREVENTSTREAM_H
