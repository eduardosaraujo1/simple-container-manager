#include <QObject>

#ifndef MAINWINDOWVMODEL_H
#define MAINWINDOWVMODEL_H

/*
 * Responsability:
 * - Manage the lifecycle of the data sources
 * - Expose UI state (ContainerTableModel)
 * - Expose the following actions:
 *   - startContainer(containerIndex)
 *   - stopContainer(containerIndex)
 *   - runAction(containerIndex)
 *   - refreshContainers()
 *   - reloadSettings()
 *
 */
class MainWindowVModel : public QObject
{
    Q_OBJECT

public:
    explicit MainWindowVModel(QObject *parent);
};

#endif // MAINWINDOWVMODEL_H
