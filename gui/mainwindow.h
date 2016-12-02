#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QTabWidget>
#include <QTimer>
#include <QtCharts>
#include <QStandardItemModel>

#include "systemthread.h"

namespace gui {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    SystemThread *mainThread = nullptr;
private slots:
    void handlePPButton();
    void handleStepButton();
    void handleSendCommand();
    void updateProcesses();
    void updateMemoryChart();
    void updateCPUChart();
    void updateIOChart();
    void queryProcess(int pid);
private:
    void log(QString s) { m_commandOutput->append(s); }

    QTabWidget *tabs;
    QWidget *term_tab, *sys_tab, *top_tab, *proc_tab;

    QPushButton *m_pp_button;
    QPushButton *m_step_button;
    QPushButton *m_submitCommand;
    QTextEdit *m_commandOutput;
    QLineEdit *m_commandInput;

    QtCharts::QChart *m_cpuUsage, *m_memUsage, *m_ioUsage;
    int cpu_lastCount=0, io_lastCount=0;

    QTableView *table;
    QStandardItemModel *tModel;

    QTimer *timer;

    bool state = true;
signals:

public slots:
};

}

#endif // MAINWINDOW_H
