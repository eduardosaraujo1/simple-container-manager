#include <QDebug>
#include "signalreceiver.h"

SignalReceiver::SignalReceiver() {}

void SignalReceiver::mySignalReceiver(const QString &message) {
    qInfo() << message;
}