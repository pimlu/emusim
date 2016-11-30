#include "mainwindow.h"

#include "sim/syscalls.h"
#include "emu/emuprocess.h"
#include "sim/process.h"

#include <stdexcept>

namespace gui {

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setFixedSize(300,400);

    m_button = new QPushButton("Resume Scheduler", this);
    m_button->setGeometry(QRect(QPoint(5, 345), QSize(290, 50)));
    m_button->show();
    connect(m_button, SIGNAL (released()), this, SLOT (handleButton()));

    m_commandOutput = new QTextEdit(this);
    m_commandOutput->setText("Welcome, type HELP for more information.");
    m_commandOutput->setGeometry(QRect(QPoint(5, 5), QSize(290, 310)));
    m_commandOutput->setReadOnly(true);

    m_commandInput = new QLineEdit(this);
    m_commandInput->setGeometry(QRect(QPoint(5, 320), QSize(240, 20)));
    connect(m_commandInput, SIGNAL (returnPressed()), this, SLOT (handleSendCommand()));

    m_submitCommand = new QPushButton(">", this);
    m_submitCommand->setGeometry(QRect(QPoint(250, 318), QSize(45, 23)));
    connect(m_submitCommand, SIGNAL (released()), this, SLOT (handleSendCommand()));
}

void MainWindow::handleButton() {
    if(!mainThread) throw std::runtime_error("mainThread is null");
    m_button->setText(mainThread->toggle() ? "Resume Scheduler" : "Pause Scheduler");
}

void MainWindow::handleSendCommand()
{
    if(!mainThread) throw std::runtime_error("mainThread is null");

    log(">" + m_commandInput->text());

    QString command = m_commandInput->text().toLower();

    if(command == "exec")
    {
        log("Currently unimplemented, use EMU_TEST in the meantime.");
    }
    else if(command == "emu_test")
    {
        char *memory = new char[0x10000];

        // Hello World, prints "Hello, world!" to the screen
        unsigned char program[] =
        {
            0xc1, 0x7f, 0x48, 0x00, 0x00, 0x10, 0xc1, 0x7f,
            0x65, 0x00, 0x01, 0x10, 0xc1, 0x7f, 0x6c, 0x00,
            0x02, 0x10, 0xc1, 0x7f, 0x6c, 0x00, 0x03, 0x10,
            0xc1, 0x7f, 0x6f, 0x00, 0x04, 0x10, 0xc1, 0x7f,
            0x2c, 0x00, 0x05, 0x10, 0xc1, 0x7f, 0x20, 0x00,
            0x06, 0x10, 0xc1, 0x7f, 0x77, 0x00, 0x07, 0x10,
            0xc1, 0x7f, 0x6f, 0x00, 0x08, 0x10, 0xc1, 0x7f,
            0x72, 0x00, 0x09, 0x10, 0xc1, 0x7f, 0x6c, 0x00,
            0x0a, 0x10, 0xc1, 0x7f, 0x64, 0x00, 0x0b, 0x10,
            0xc1, 0x7f, 0x21, 0x00, 0x0c, 0x10, 0xc1, 0xaf,
            0x0d, 0x10, 0x01, 0x7c, 0x00, 0x10, 0x21, 0xbc,
            0x00, 0x7d, 0x02, 0x00, 0x81, 0x7f, 0x2e, 0x00
        };

        // Copy program into processes memory
        memcpy(memory, program, sizeof(program));

        // Create process and schedule it
        emu::EmuProcess *emu = new emu::EmuProcess((char*) memory, sizeof(memory));
        mainThread->system->sched->add(emu);

        log("Scheduled emu test program");
    }
    else if(command == "help")
    {
        log("Current list of commands: EXEC, EMU_TEST");
    }
    else
    {
        log("Unrecognized command: " + command);
    }

    m_commandInput->setText("");
}

}
