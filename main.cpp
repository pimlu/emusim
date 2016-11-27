#include <QApplication>

#include "gui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    gui::MainWindow mainWindow;
    mainWindow.show();
    return a.exec();
}
