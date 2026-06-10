#ifndef SIGNALTRANSMITTER_H
#define SIGNALTRANSMITTER_H

#include <QObject>

class SignalTransmitter : public QObject
{
    Q_OBJECT
public:
    SignalTransmitter();
signals:
    void myFirstSignal(const QString &message);
};

#endif // SIGNALTRANSMITTER_H
