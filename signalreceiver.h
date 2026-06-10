#ifndef SIGNALRECEIVER_H
#define SIGNALRECEIVER_H

#include <QObject>

class SignalReceiver : public QObject
{
    Q_OBJECT
public:
    SignalReceiver();
public slots:
    void mySignalReceiver(const QString &message);
};

#endif // SIGNALRECEIVER_H
