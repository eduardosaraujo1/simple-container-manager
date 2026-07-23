#ifndef CONTAINERACTIONRUNNER_H
#define CONTAINERACTIONRUNNER_H

#include <QObject>

class ContainerActionRunner : public QObject {
    Q_OBJECT
public:
    explicit ContainerActionRunner(QObject* parent = nullptr);
}

#endif //CONTAINERACTIONRUNNER_H