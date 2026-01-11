// ============================================
// FILE: main_gui.cpp
// DESCRIPTION: GUI Application Entry Point
// ============================================

#include "../../include/gui/mainwindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Application metadata
    QApplication::setApplicationName("Data Center Simulator");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("OS Course Project");
    
    // Set fusion style for modern look (optional)
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    
    // Create and show main window
    MainWindow window;
    window.show();
    
    return app.exec();
}