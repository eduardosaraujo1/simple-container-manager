#ifndef CONTAINERROWWIDGET_H
#define CONTAINERROWWIDGET_H

#include <QWidget>

namespace Ui {
class ContainerRowWidget;
}

class ContainerRowWidget : public QWidget
{
    Q_OBJECT

public:
    enum class Status {
        Running,
        Stopped,
        Loading,
        Error
    };

    explicit ContainerRowWidget(
        const QString& iconPath,
        const QString& label,
        Status status,
        const QString& action,
        QWidget *parent = nullptr
    );

    ~ContainerRowWidget();

    void setIcon(const QString& iconPath);
    void setLabel(const QString& label);
    void setStatus(Status status);
    void setAction(const QString& action);

signals:
    void toggleRequested(bool start);
    void adminRequested(QString action);

private slots:
    void onToggleClicked();
    void onAdminClicked();

private:
    void updateStatusStyle();

    Ui::ContainerRowWidget *ui;

    Status m_status;
    QString m_action;
};
#endif // CONTAINERROWWIDGET_H
