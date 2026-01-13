#include <QApplication>
#include <QFile>
#include <QDebug>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    qDebug() << "Starting application...";
    QApplication a(argc, argv);
    a.setApplicationName("EdgeX Qt Console");
    a.setOrganizationName("EdgeX-UI");

    // Load Theme from Resources
    QFile file(":/ModernTheme.qss");
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        a.setStyleSheet(styleSheet);
        qDebug() << "Theme loaded successfully from resources";
    } else {
        qDebug() << "Failed to load theme from resources, trying local file...";
        QFile localFile("ModernTheme.qss");
        if (localFile.open(QFile::ReadOnly)) {
            a.setStyleSheet(QLatin1String(localFile.readAll()));
            qDebug() << "Theme loaded from local file";
        } else {
            qDebug() << "Failed to load theme completely";
        }
    }

    MainWindow w;
    w.show();
    return a.exec();
}
