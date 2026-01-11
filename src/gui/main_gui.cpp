#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QApplication::setApplicationName("Data Center Simulator");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("OS Course Project");
    
    MainWindow window;
    window.show();
    
    return app.exec();
}