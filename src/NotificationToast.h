#ifndef NOTIFICATIONTOAST_H
#define NOTIFICATIONTOAST_H

#include <QWidget>
#include <QLabel>
#include <QTimer>

class NotificationToast : public QWidget
{
    Q_OBJECT
public:
    explicit NotificationToast(const QString &category, const QString &content, QWidget *parent = nullptr);
    static void showToast(const QString &category, const QString &content, QWidget *parent);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QLabel *m_label;
    QTimer *m_timer;
    float m_opacity;
};

#endif // NOTIFICATIONTOAST_H
