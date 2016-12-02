#include "mainwindow.h"

#include "sim/syscalls.h"
#include "emu/emuprocess.h"
#include "sim/process.h"
#include "emu/emuprocess.h"

#include <stdexcept>
#include <string>
#include <vector>
#include <QList>
#include <QBoxLayout>

namespace gui {

//enum ProcStatus { JOB, RUNNING, WAITING, BLOCKED, ENDED };
QStringList statuses = {"job", "run", "wait", "blocked", "ended"};

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
    m_pp_button->setGeometry(QRect(QPoint(5, 445), QSize(775, 50)));
    m_pp_button->show();
    connect(m_pp_button, SIGNAL (released()), this, SLOT (handlePPButton()));


    m_step_button = new QPushButton("Step", term_tab);
    m_step_button->setGeometry(QRect(QPoint(5, 500), QSize(775, 50)));
    m_step_button->show();
    connect(m_step_button, SIGNAL (released()), this, SLOT (handleStepButton()));


    m_commandOutput = new QTextEdit(term_tab);
    m_commandOutput->setText("Welcome, type HELP for more information.");
    m_commandOutput->setGeometry(QRect(QPoint(5, 5), QSize(775, 410)));
    m_commandOutput->setReadOnly(true);


    m_commandInput = new QLineEdit(term_tab);
    m_commandInput->setGeometry(QRect(QPoint(5, 420), QSize(725, 20)));
    connect(m_commandInput, SIGNAL (returnPressed()), this, SLOT (handleSendCommand()));


    m_submitCommand = new QPushButton(">", term_tab);
    m_submitCommand->setGeometry(QRect(QPoint(735, 418), QSize(45, 23)));
    connect(m_submitCommand, SIGNAL (released()), this, SLOT (handleSendCommand()));


    // placeholder chart for memory
    QLineSeries *mem_usage = new QLineSeries();

    m_memUsage = new QChart();
    m_memUsage->legend()->hide();
    m_memUsage->addSeries(mem_usage);
    m_memUsage->createDefaultAxes();
    m_memUsage->setTitle("Memory Usage");

    QChartView *mem_chartView = new QChartView(m_memUsage, sys_tab);
    mem_chartView->setGeometry(QRect(QPoint(0, 0), QSize(263, 225)));
    mem_chartView->setRenderHint(QPainter::Antialiasing);
    mem_chartView->setAutoFillBackground(true);

    QTimer *mem_timer = new QTimer(this);
    connect(mem_timer, SIGNAL(timeout()), this, SLOT(updateMemoryChart()));
    mem_timer->start(1000);


    // place holder chart for CPU usage
    QLineSeries *cpu_usage = new QLineSeries();

    m_cpuUsage = new QChart();
    m_cpuUsage->legend()->hide();
    m_cpuUsage->addSeries(cpu_usage);
    m_cpuUsage->createDefaultAxes();
    m_cpuUsage->setTitle("CPU Usage (Cycles per chart update)");

    QChartView *cpu_chartView = new QChartView(m_cpuUsage, sys_tab);
    cpu_chartView->setGeometry(QRect(QPoint(263, 0), QSize(263, 225)));
    cpu_chartView->setRenderHint(QPainter::Antialiasing);

    QTimer *cpu_timer = new QTimer(this);
    connect(cpu_timer, SIGNAL(timeout()), this, SLOT(updateCPUChart()));
    cpu_timer->start(1000);


    // place holder chart for IO requests
    QLineSeries *io_usage = new QLineSeries();

    m_ioUsage = new QChart();
    m_ioUsage->legend()->hide();
    m_ioUsage->addSeries(io_usage);
    m_ioUsage->createDefaultAxes();
    m_ioUsage->setTitle("IO Requests");

    QChartView *io_chartView = new QChartView(m_ioUsage, sys_tab);
    io_chartView->setGeometry(QRect(QPoint(526, 0), QSize(263, 225)));
    io_chartView->setRenderHint(QPainter::Antialiasing);

    QTimer *io_timer = new QTimer(this);
    connect(io_timer, SIGNAL(timeout()), this, SLOT(updateIOChart()));
    io_timer->start(1000);

    // ----

    QBoxLayout *fillTop = new QVBoxLayout(top_tab);
    table = new QTableView(top_tab);
    table->setSortingEnabled(true);
    tModel = new QStandardItemModel();
    tModel->setHorizontalHeaderLabels(QStringList {"PID","status","name","memory","IO"});
    table->setModel(tModel);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
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

    if(command == "load")
    {
        if(line.size() < 2) {
            log("No file given; run `load fileName`");
        } else {
            std::string file = line[1].toUtf8().constData();
            int pid = mainThread->exec(file);
            log(QString::fromStdString(pid == -1 ? "File "+file+" not found." : "Running "+
                             file+" as PID "+std::to_string(pid)+"."));
        }
    }
    else if(command == "kill")
    {
        if(line.size() < 2) {
            log("No file given; run `load fileName`");
        } else {
            int pid = line[1].toInt();
            mainThread->remove(pid);
        }
    }
    else if(command == "proc")
    {
        std::vector<ProcData> stats = mainThread->getProcs();

        log("Processes");
        log("---------");
        for(ProcData pd : stats)
        {
            log(QString("[%1: %2] \t Status: %3, Memory: %4, IO Requests: %5")
            .arg
            (
                    QString::number(pd.pcb.pid),
                    QString::fromStdString(pd.pcb.name),
                    statuses[pd.status],
                    QString::number(pd.memory),
                    QString::number(pd.pcb.ioreqs)
            ));
        }
    }
    else if(command == "mem")
    {
        float percent = ((float) mainThread->system->usedMem / (float) mainThread->system->memory) * 100;

        log(QString("Currently using %1 out of %2 available memory (%3%).")
        .arg
        (
                QString::number(mainThread->system->usedMem),
                QString::number(mainThread->system->memory),
                QString::number(percent)
        ));
    }
    else if(command == "exe")
    {
        if(line.size() > 1)
        {
            int steps = line[1].toInt();
            mainThread->step(steps);
            log("Stepped " + line[1] + " cycles.");
        }
        else
        {
            handlePPButton();
        }
    }
    else if(command == "reset")
    {
        mainThread->reset();
        log("Simulator reset.");
    }
    else if(command == "exit")
    {
        exit(1);
    }
    else if(command == "inspect")
    {
        if(line.size() < 2) log("Usage: INSPECT <PID>");
        else
        {
            queryProcess(line[1].toInt());
        }
    }
    else if(command == "help")
    {
        log("Current list of commands: PROC, MEM, LOAD, EXE, RESET, EXIT, INSPECT, HELP");
        log("");
        log("Special usages:");
        log("\t EXE, EXE <# of steps>");
        log("\t LOAD <binary name/file name>");
        log("\t INSPECT <PID>");
    }
    else
    {
        log("Unrecognized command: " + command);
    }

}

void MainWindow::updateMemoryChart()
{
    QLineSeries *mem_usage = dynamic_cast<QLineSeries*>(m_memUsage->series().at(0));

    int max = mem_usage->points().size();

    // Shift all points over, usually I'd use auto-foreach, but cant seem to do that
    // here for some reason (wont update the points x value)
    if(max > 5)
    {
        mem_usage->removePoints(0, 1);

        for(int i = 0; i < mem_usage->points().size(); i++)
        {
           QPointF p = mem_usage->at(i);
           p.setX(i);
           mem_usage->removePoints(i, 1);
           mem_usage->insert(i, p);
        }
    }

    // Add new point
    mem_usage->append(max, ((float) mainThread->system->usedMem / (float) mainThread->system->memory) * 100);

    // Update chart
    m_memUsage->removeSeries(mem_usage);
    m_memUsage->addSeries(mem_usage);

    m_memUsage->createDefaultAxes();
    m_memUsage->axisX()->setRange(0, max);
    m_memUsage->axisY()->setRange(0, 100);

    dynamic_cast<QValueAxis*>(m_memUsage->axisX())->setVisible(false);
}

void MainWindow::updateCPUChart()
{
    QLineSeries *cpu_usage = dynamic_cast<QLineSeries*>(m_cpuUsage->series().at(0));

    int max = cpu_usage->points().size();

    // Shift all points over, usually I'd use auto-foreach, but cant seem to do that
    // here for some reason (wont update the points x value)
    if(max > 5)
    {
        cpu_usage->removePoints(0, 1);

        for(int i = 0; i < cpu_usage->points().size(); i++)
        {
           QPointF p = cpu_usage->at(i);
           p.setX(i);
           cpu_usage->removePoints(i, 1);
           cpu_usage->insert(i, p);
        }
    }

    // Add new point
    int cycles = 0;
    std::vector<ProcData> stats = mainThread->getProcs();

    for(ProcData pd : stats) cycles += pd.pcb.cycles;

    cpu_usage->append(max, std::max(0, cycles - cpu_lastCount));
    cpu_lastCount = cycles;

    // Update chart
    m_cpuUsage->removeSeries(cpu_usage);
    m_cpuUsage->addSeries(cpu_usage);

    m_cpuUsage->createDefaultAxes();
    m_cpuUsage->axisX()->setRange(0, max);
    m_cpuUsage->axisY()->setMin(0);

    dynamic_cast<QValueAxis*>(m_cpuUsage->axisX())->setVisible(false);
}

void MainWindow::updateIOChart()
{
    QLineSeries *io_usage = dynamic_cast<QLineSeries*>(m_ioUsage->series().at(0));

    int max = io_usage->points().size();

    // Shift all points over, usually I'd use auto-foreach, but cant seem to do that
    // here for some reason (wont update the points x value)
    if(max > 5)
    {
        io_usage->removePoints(0, 1);

        for(int i = 0; i < io_usage->points().size(); i++)
        {
           QPointF p = io_usage->at(i);
           p.setX(i);
           io_usage->removePoints(i, 1);
           io_usage->insert(i, p);
        }
    }

    // Add new point
    int requests = 0;
    std::vector<ProcData> stats = mainThread->getProcs();

    for(ProcData pd : stats) requests += pd.pcb.ioreqs;

    io_usage->append(max, std::max(0, requests - io_lastCount));
    io_lastCount = requests;

    // Update chart
    m_ioUsage->removeSeries(io_usage);
    m_ioUsage->addSeries(io_usage);

    m_ioUsage->createDefaultAxes();
    m_ioUsage->axisY()->setMin(0);

    dynamic_cast<QValueAxis*>(m_ioUsage->axisX())->setVisible(false);
}

void MainWindow::updateProcesses() {
    std::vector<ProcData> stats = mainThread->getProcs();
    tModel->removeRows(0, tModel->rowCount());
    for(ProcData pd : stats) {
        const QList<QStandardItem*> row(
            {new NumericStdItem(pd.pcb.pid), new QStandardItem(statuses[pd.status]),
             new QStandardItem(QString::fromStdString(pd.pcb.name)),
             new NumericStdItem(pd.memory), new NumericStdItem(pd.pcb.ioreqs)});
        tModel->appendRow(row);
    }
    //re-sort it to fix appending rows killing ordering (better way?)
    QHeaderView *header = table->horizontalHeader();
    table->sortByColumn(header->sortIndicatorSection(), header->sortIndicatorOrder());
}
//call this when someone queries a process
void MainWindow::queryProcess(int pid)
{
    ulock_recmtx lck = mainThread->getLock();
    sim::Scheduler *sched = mainThread->system->sched;
    emu::EmuProcess *p = dynamic_cast<emu::EmuProcess*>(sched->find(pid));

    // Check if process exists
    if(!p) return;

    sim::PCB &pcb = sched->pcbs[p];

    for(ProcData pd : mainThread->getProcs())
    {
        if(pd.pcb.pid == pid)
        {
            log(QString::fromUtf8(p->printRegisters()));
            log(QString("Status: %1, Memory: %2, IO Requests: %3, Name: %4, Cycles Completed: %5")
            .arg
            (
                    statuses[pd.status],
                    QString::number(pd.memory),
                    QString::number(pcb.ioreqs),
                    QString::fromStdString(pd.pcb.name),
                    QString::number(pcb.cycles)
            ));

            break;
        }
    }
}

}
