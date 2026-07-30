#ifndef CONTAINERROWWIDGET_H
#define CONTAINERROWWIDGET_H

#include <QWidget>

namespace Ui
{
    class ContainerRowWidget;
}

class ContainerRowWidget : public QWidget
{
    Q_OBJECT

public:
    enum class Status
    {
        Started,
        Stopped,
        Loading,
        Starting,
        Stopping,
        NotFound,
        Unknown
    };

    explicit ContainerRowWidget(
        const QString &label,
        const QString &icon,
        bool hasAction,
        Status status,
        QWidget *parent = nullptr);

    ~ContainerRowWidget();

    Status status() const;

    void setIcon(const QString &iconPath);
    void setLabel(const QString &label);
    void setStatus(Status status);
    void setHasAction(bool hasAction);

signals:
    void toggleRequested(bool start);
    void adminRequested();

private slots:
    void onToggleClicked();
    void onAdminClicked();

private:
    void updateStatusStyle();

    Ui::ContainerRowWidget *ui;

    Status m_status;
};
#endif // CONTAINERROWWIDGET_H
