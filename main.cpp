#include <QApplication>

#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UI::MainWindow mainWindow;
    mainWindow.show();
    return a.exec();
}
