#include <QApplication>
#include <iostream>

#include "sim/system.h"
#include "gui/mainwindow.h"
#include "gui/systemthread.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    sim::System *mainSystem = new sim::System(65536, 200, std::cin, std::cout);
    gui::SystemThread *mainThread = new gui::SystemThread(mainSystem, 10000);

    gui::MainWindow mainWindow;
    mainWindow.mainThread = mainThread;
    mainWindow.show();

    int ret = a.exec();
    delete mainThread;
    delete mainSystem;
    return ret;
}
