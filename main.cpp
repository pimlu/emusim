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
   unsigned char memory[] = { 0x01, 0x84, 0x21, 0x88, 0x41, 0xd4, 0x52, 0x84,
                              0x81, 0x7f, 0x0d, 0x00, 0x61, 0x00, 0x62, 0x04,
                              0x01, 0x04, 0x21, 0x0c, 0x43, 0x88, 0x81, 0x7f,
                              0x03, 0x00, 0x00, 0x7d, 0x01, 0x00 };
   int cycles = 500;

   emu::EmuProcess emu((char*) memory, sizeof(memory));
   emu.run(cycles, new sim::Sysres(sim::Type::NONE));
   // ---


    int ret = a.exec();
    delete mainThread;
    delete mainSystem;
    return ret;
}
