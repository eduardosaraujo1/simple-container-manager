#ifndef CONTAINERLISTWIDGET_H
#define CONTAINERLISTWIDGET_H

#include <QHash>
#include <QWidget>

#include <data/apppreferences.h>
#include <data/data-objects/containerinfo.h>
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

    void initialize(const QList<AppPreferences::ContainerSpec> &containers);
public slots:
    void refreshContainerInfo(const QList<ContainerInfo> &containers);

signals:
    void containerToggle(const QString &containerName, bool isStartCommand);
    void containerAction(const QString &containerName, const QString &action);

private:
    static ContainerRowWidget::Status mapStatus(ContainerInfo::Status status);

    Ui::ContainerListWidget *ui;

    bool m_initialized = false;
    QHash<QString, ContainerRowWidget*> m_rows;
};

#endif // CONTAINERLISTWIDGET_H