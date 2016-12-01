#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QTabWidget>
#include <QtCharts>

#include "systemthread.h"

namespace gui {

using namespace QtCharts;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    SystemThread *mainThread = nullptr;
private slots:
    void handlePPButton();
    void handleStepButton();
    void handleSendCommand();
private:
    void log(QString s) { m_commandOutput->append(s); }

    QTabWidget *tabs;
    QWidget *term_tab, *top_tab, *proc_tab;

    QPushButton *m_pp_button;
    QPushButton *m_step_button;
    QPushButton *m_submitCommand;
    QTextEdit *m_commandOutput;
    QLineEdit *m_commandInput;

    QChart *m_cpuUsage, *m_memUsage;

    bool state = true;
signals:

public slots:
};

}

#endif // MAINWINDOW_H
