#include "mainwindow.h"

#include "sim/syscalls.h"
#include "emu/emuprocess.h"
#include "sim/process.h"


#include <stdexcept>
#include <string>
#include <vector>
#include <QList>
#include <QBoxLayout>

namespace gui {

using namespace QtCharts;

class NumericStdItem : public QStandardItem {
public:
    NumericStdItem(int n) : QStandardItem(QString::number(n)){}

    bool operator <(const QStandardItem &other) const
    {
        return text().toInt() < other.text().toInt();
    }
};


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setFixedSize(800,600);
    tabs = new QTabWidget(this);
    tabs->setGeometry(QRect(QPoint(5, 5), QSize(790, 580)));
    tabs->addTab(term_tab = new QWidget, tr("Terminal"));
    tabs->addTab(sys_tab = new QWidget, tr("System"));
    tabs->addTab(top_tab = new QWidget, tr("Processes"));
    tabs->addTab(proc_tab = new QWidget, tr("Inspector"));

    term_tab->show();

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


    // placeholder chart for memory
    QLineSeries *mem_usage = new QLineSeries();
    mem_usage->append(1, 10);
    mem_usage->append(2, 10);
    mem_usage->append(3, 20);
    mem_usage->append(4, 10);


    m_memUsage = new QChart();
    m_memUsage->legend()->hide();
    m_memUsage->addSeries(mem_usage);
    m_memUsage->createDefaultAxes();
    m_memUsage->setTitle("Memory Usage");

    QChartView *mem_chartView = new QChartView(m_memUsage, sys_tab);
    mem_chartView->setGeometry(QRect(QPoint(0, 0), QSize(300, 225)));
    mem_chartView->setRenderHint(QPainter::Antialiasing);

    // place holder chart for cpu
    QLineSeries *cpu_usage = new QLineSeries();
    cpu_usage->append(1, 10);
    cpu_usage->append(2, 10);
    cpu_usage->append(3, 20);
    cpu_usage->append(4, 10);

    m_cpuUsage = new QChart();
    m_cpuUsage->legend()->hide();
    m_cpuUsage->addSeries(cpu_usage);
    m_cpuUsage->createDefaultAxes();
    m_cpuUsage->setTitle("CPU Usage");

    QChartView *cpu_chartView = new QChartView(m_cpuUsage, sys_tab);
    cpu_chartView->setGeometry(QRect(QPoint(0, 215), QSize(300, 240)));
    cpu_chartView->setRenderHint(QPainter::Antialiasing);

    QBoxLayout *fillTop = new QVBoxLayout(top_tab);
    table = new QTableView(top_tab);
    table->setSortingEnabled(true);
    tModel = new QStandardItemModel();
    tModel->setHorizontalHeaderLabels(QStringList {"PID","name","memory","IO"});
    table->setModel(tModel);
    fillTop->addWidget(table, 1);
    tabs->show();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateProcesses()));
    timer->start(1000);
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
    else if(command == "add_point_test")
    {
        QLineSeries *cpu_usage = new QLineSeries();
        cpu_usage->append(5, 10);
        cpu_usage->append(6, 5);
        cpu_usage->append(7, 10);
        cpu_usage->append(8, 5);
        m_cpuUsage->addSeries(cpu_usage);
        m_cpuUsage->createDefaultAxes();
    }
    else if(command == "help")
    {
        log("Current list of commands: EXEC, EMU_TEST, ADD_POINT_TEST");
    }
    else
    {
        log("Unrecognized command: " + command);
    }

}


void MainWindow::updateProcesses() {
    std::vector<ProcData> stats = mainThread->getProcs();
    tModel->removeRows(0, tModel->rowCount());
    for(ProcData pd : stats) {
        const QList<QStandardItem*> row(
            {new NumericStdItem(pd.pcb.pid), new QStandardItem(QString::fromStdString(pd.pcb.name)),
             new NumericStdItem(pd.memory), new NumericStdItem(pd.pcb.ioreqs)});
        tModel->appendRow(row);
    }
    table->update();
}

}
