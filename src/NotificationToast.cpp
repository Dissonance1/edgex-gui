#include "NotificationToast.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QPropertyAnimation>
#include <QWindow>

NotificationToast::NotificationToast(const QString &category, const QString &content, QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setAttribute(Qt::WA_X11DoNotAcceptFocus); // For X11 environments like Aetina

    QVBoxLayout *layout = new QVBoxLayout(this);
    
    QLabel *catLabel = new QLabel(category.toUpper());
    catLabel->setStyleSheet("color: #FF8800; font-weight: bold; font-size: 10px;");
    layout->addWidget(catLabel);

    m_label = new QLabel(content);
    m_label->setStyleSheet("color: white; font-size: 13px;");
    m_label->setWordWrap(true);
    layout->addWidget(m_label);

    setStyleSheet("background-color: rgba(30, 30, 45, 230); border: 1px solid #FF8800; border-radius: 8px; padding: 10px;");

    setFixedWidth(300);
    adjustSize();

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, [this]() {
        QPropertyAnimation *anim = new QPropertyAnimation(this, "windowOpacity");
        anim->setDuration(500);
        anim->setStartValue(1.0);
        anim->setEndValue(0.0);
        connect(anim, &QPropertyAnimation::finished, this, &NotificationToast::deleteLater);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    });
    m_timer->start(5000);
}

void NotificationToast::showToast(const QString &category, const QString &content, QWidget *parent)
{
    NotificationToast *toast = new NotificationToast(category, content, parent);
    toast->setAttribute(Qt::WA_DeleteOnClose);
    
    // Position at top-right of the screen
    QScreen *screen = QApplication::primaryScreen();
    if (parent && parent->windowHandle()) {
        screen = parent->windowHandle()->screen();
    }
    
    QRect screenRect = screen->geometry();
    int x = screenRect.right() - toast->width() - 20;
    int y = screenRect.top() + 40;

    toast->move(x, y);
    toast->show();
    qDebug() << "Toast shown at" << x << "," << y << "Content:" << content.left(20);
}

void NotificationToast::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
}

void NotificationToast::enterEvent(QEnterEvent *event)
{
    m_timer->stop();
    setWindowOpacity(1.0);
}

void NotificationToast::leaveEvent(QEvent *event)
{
    m_timer->start(2000);
}
