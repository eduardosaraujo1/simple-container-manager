#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <data/data-objects/configuredcontainers.h>
#include <domain/containerservice.h>

#include <containerlistwidget.h>

class ContainerService;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * Coordinates presentation between the UI and ContainerService.
 *
 * MainWindow owns only presentation logic.
 * Docker communication and container workflows are delegated to
 * ContainerService.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(
        ContainerService &containerService,
        QWidget *parent = nullptr);
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private:
    void initializeContainers(const ConfiguredContainers &containers);

    void setupConnections();

    void setWarning(const QString &warning);

private slots:
    void onExitClicked();
    void onRefreshClicked();
    void onContainerToggleRequested(
        const QString &containerName, bool isStartCommand);
    void onContainerActionRequested(const QString &containerName);
    void onContainersUpdated(const QList<ContainerState> &containers);
    void onAutoRefreshDown();

private:
    Ui::MainWindow *ui;

    ContainerService *m_containerService = nullptr;
    ContainerListWidget *m_listWidget = nullptr;
};

#endif