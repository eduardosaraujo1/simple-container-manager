#ifndef CONTAINERLISTWIDGET_H
#define CONTAINERLISTWIDGET_H

#include <QHash>
#include <QWidget>

#include <data/appconfigreader.h>
#include <data/data-objects/containerinfo.h>
#include <data/data-objects/appprefs/containerspec.h>
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

    void initialize(const QList<ContainerSpec> &containers);
public slots:
    void refreshContainerInfo(const QList<ContainerInfo> &containers);

signals:
    void containerToggle(const QString &containerName, bool isStartCommand);
    void containerAction(const QString &containerName);

private:
    static ContainerRowWidget::Status mapStatus(ContainerInfo::Status status);

    Ui::ContainerListWidget *ui;

    bool m_initialized = false;

    QHash<QString, ContainerRowWidget*> m_rows;
};

#endif // CONTAINERLISTWIDGET_H