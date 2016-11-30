#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>

#include "systemthread.h"

namespace gui {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    SystemThread *mainThread = nullptr;
private slots:
    void handleButton();
    void handleSendCommand();
private:
    void log(QString s) { m_commandOutput->append(s); }

    QPushButton *m_button;
    QPushButton *m_submitCommand;
    QTextEdit *m_commandOutput;
    QLineEdit *m_commandInput;

    bool state = true;
signals:

public slots:
};

}

#endif // MAINWINDOW_H
