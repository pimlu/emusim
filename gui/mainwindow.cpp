#include "mainwindow.h"

#include "sim/syscalls.h"
#include "emu/emuprocess.h"
#include "sim/process.h"

#include <stdexcept>
#include <string>

namespace gui {

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setFixedSize(315,495);


    tabs = new QTabWidget(this);
    tabs->setGeometry(QRect(QPoint(5, 5), QSize(305, 480)));
    tabs->addTab(term_tab = new QWidget, tr("Terminal"));
    tabs->addTab(top_tab = new QWidget, tr("System"));
    tabs->addTab(proc_tab = new QWidget, tr("Process"));

    term_tab->show(); top_tab->show(); proc_tab->show();

    m_pp_button = new QPushButton("Resume Scheduler", term_tab);
    m_pp_button->setGeometry(QRect(QPoint(5, 345), QSize(290, 50)));
    m_pp_button->show();
    connect(m_pp_button, SIGNAL (released()), this, SLOT (handlePPButton()));

    m_step_button = new QPushButton("Step", term_tab);
    m_step_button->setGeometry(QRect(QPoint(5, 400), QSize(290, 50)));
    m_step_button->show();
    connect(m_step_button, SIGNAL (released()), this, SLOT (handleStepButton()));

    m_commandOutput = new QTextEdit(term_tab);
    m_commandOutput->setText("Welcome, type HELP for more information.");
    m_commandOutput->setGeometry(QRect(QPoint(5, 5), QSize(290, 310)));
    m_commandOutput->setReadOnly(true);

    m_commandInput = new QLineEdit(term_tab);
    m_commandInput->setGeometry(QRect(QPoint(5, 320), QSize(240, 20)));
    connect(m_commandInput, SIGNAL (returnPressed()), this, SLOT (handleSendCommand()));

    m_submitCommand = new QPushButton(">", term_tab);
    m_submitCommand->setGeometry(QRect(QPoint(250, 318), QSize(45, 23)));
    connect(m_submitCommand, SIGNAL (released()), this, SLOT (handleSendCommand()));

    tabs->show();
}

void MainWindow::handlePPButton() {
    if(!mainThread) throw std::runtime_error("mainThread is null");
    bool paused = mainThread->toggle() ;
    m_pp_button->setText(paused ? "Resume Scheduler" : "Pause Scheduler");
    m_step_button->setDisabled(!paused);
}
void MainWindow::handleStepButton() {
    if(!mainThread) throw std::runtime_error("mainThread is null");
    unsigned long long steps = mainThread->step(10);
    log(QString::fromStdString("Stepped "+std::to_string(steps)+" cycles."));
}

void MainWindow::handleSendCommand()
{
    if(!mainThread) throw std::runtime_error("mainThread is null");

    log(">" + m_commandInput->text());

    QStringList line = m_commandInput->text().split(" ");
    m_commandInput->setText("");
    if(line.size() == 0) return;
    QString command = line[0].toLower();

    if(command == "exec")
    {
        if(line.size() < 2) {
            log("No file given; run `exec fileName`");
        } else {
            std::string file = line[1].toUtf8().constData();
            int pid = mainThread->exec(file);
            log(QString::fromStdString(pid == -1 ? "File "+file+" not found." : "Running "+
                             file+" as PID "+std::to_string(pid)+"."));
        }
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
        mainThread->add(emu, "emu");

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

}

}
