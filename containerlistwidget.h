#ifndef CONTAINERLISTWIDGET_H
#define CONTAINERLISTWIDGET_H

#include <QHash>
#include <QWidget>

#include <data/appconfigreader.h>
#include <data/data-objects/containerstate.h>
#include <data/data-objects/containerdefinition.h>
#include "containerrowwidget.h"

namespace Ui {
class ContainerListWidget;
}

class ContainerListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContainerListWidget(QWidget *parent = nullptr);
    ~ContainerListWidget();

    void initialize(const QList<ContainerDefinition> &containers);
public slots:
    void refreshContainerInfo(const QList<ContainerState> &containers);
    void setContainerStatus(const QString &containerName, ContainerRowWidget::Status status);

signals:
    void containerToggle(const QString &containerName, bool isStartCommand);
    void containerAction(const QString &containerName);
private:
    static ContainerRowWidget::Status mapStatus(ContainerState::Status status);

    Ui::ContainerListWidget *ui;

    bool m_initialized = false;

    QHash<QString, ContainerRowWidget*> m_rows;
};

#endif // CONTAINERLISTWIDGET_H