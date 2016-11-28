#include <QApplication>
#include <iostream>

#include "sim/system.h"
#include "gui/mainwindow.h"
#include "gui/systemthread.h"

#include "sim/syscalls.h"
#include "emu/emuprocess.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    sim::System *mainSystem = new sim::System(65536, 200, std::cin, std::cout);
    gui::SystemThread *mainThread = new gui::SystemThread(mainSystem, 10000);

    gui::MainWindow mainWindow;
    mainWindow.mainThread = mainThread;
    mainWindow.show();

    // Temporary until we have File IO added
   char memory[] = { 0x02, 0x88, 0x00, 0x7d, 0x01, 0x00 };
   int cycles = 5;

   emu::EmuProcess emu(memory, sizeof(memory));
   emu.run(cycles, new sim::Sysres(sim::Type::NONE));
   // ---


    int ret = a.exec();
    delete mainThread;
    delete mainSystem;
    return ret;
}
