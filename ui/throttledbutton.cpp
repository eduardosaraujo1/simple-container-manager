#include "throttledbutton.h"

ThrottledButton::ThrottledButton(QWidget *parent)
    : QPushButton(parent)
{
    m_timer.setSingleShot(true);

    connect(this, &QPushButton::clicked,
            this, &ThrottledButton::handleClicked);

    connect(&m_timer, &QTimer::timeout,
            this, &ThrottledButton::reenable);
}

int ThrottledButton::throttleMilliseconds() const
{
    return m_throttleMilliseconds;
}

void ThrottledButton::setThrottleMilliseconds(int milliseconds)
{
    m_throttleMilliseconds = milliseconds;
}

void ThrottledButton::handleClicked()
{
    setEnabled(false);
    m_timer.start(m_throttleMilliseconds);
}

void ThrottledButton::reenable()
{
    if (m_timer.isActive()) {
        m_timer.stop();
    }

    setEnabled(true);
}