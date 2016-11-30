#include <QApplication>
#include <iostream>

#include "sim/system.h"
#include "gui/mainwindow.h"
#include "gui/systemthread.h"

#include "sim/syscalls.h"
#include "emu/emuprocess.h"
#include "sim/process.h"

#include "sim/filesystem.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    sim::Process *p = new sim::DummyProcess(200);
    sim::System *mainSystem = new sim::System(65536, 200, std::cin, std::cout);
    mainSystem->sched->add(p);

    gui::SystemThread *mainThread = new gui::SystemThread(mainSystem, 10000);

    gui::MainWindow mainWindow;
    mainWindow.mainThread = mainThread;
    mainWindow.show();

    // Temporary until we have File IO added
    // Fibonaci Calcutator, calculates n = 20 and stores it in register A
    // https://gist.github.com/ehaliewicz/2523345
   unsigned char memory[] = { 0x01, 0x84, 0x21, 0x88, 0x41, 0xd4, 0x52, 0x84,
                              0x81, 0x7f, 0x0d, 0x00, 0x61, 0x00, 0x62, 0x04,
                              0x01, 0x04, 0x21, 0x0c, 0x43, 0x88, 0x81, 0x7f,
                              0x03, 0x00, 0x00, 0x7d, 0x01, 0x00 };
   int cycles = 500;

   emu::EmuProcess emu((char*) memory, sizeof(memory));
   emu.run(cycles, new sim::Sysres(sim::Type::NONE));
   // ---

    /*sim::FileSystem fs("foo");
    fs.writeFile("bar.txt", 3, 4, (char*) "1234");
    //std::cout.write(str, 4);
    //delete[] str;
    std::cout << std::endl;*/

    int ret = a.exec();
    delete mainThread;
    delete mainSystem;
    return ret;
}
