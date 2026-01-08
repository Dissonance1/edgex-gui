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

    // Load Theme
    QFile file("src/ModernTheme.qss");
    if (!file.open(QFile::ReadOnly)) {
        // Try current directory (deployment)
        file.setFileName("ModernTheme.qss");
        if (!file.open(QFile::ReadOnly)) {
             // Fallback for build dir structure
            file.setFileName("../src/ModernTheme.qss");
            file.open(QFile::ReadOnly);
        }
    }
    
    if (file.isOpen()) {
        QString styleSheet = QLatin1String(file.readAll());
        a.setStyleSheet(styleSheet);
        qDebug() << "Theme loaded successfully";
    } else {
        qDebug() << "Failed to load theme";
    }

    MainWindow w;
    w.show();
    return a.exec();
}
