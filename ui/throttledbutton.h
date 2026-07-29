#ifndef THROTTLEDBUTTON_H
#define THROTTLEDBUTTON_H

#include <QPushButton>
#include <QTimer>

class ThrottledButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(int throttleMilliseconds
               READ throttleMilliseconds
               WRITE setThrottleMilliseconds)

public:
    explicit ThrottledButton(QWidget *parent = nullptr);

    int throttleMilliseconds() const;
    void setThrottleMilliseconds(int milliseconds);
public slots:
    /**
     * @brief Programatically reenables the button before the timer is up
     */
    void reenable();

private slots:
    void handleClicked();

private:
    QTimer m_timer;
    int m_throttleMilliseconds = 1000;
};
#endif // THROTTLEDBUTTON_H
