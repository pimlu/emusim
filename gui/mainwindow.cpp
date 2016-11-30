#include "mainwindow.h"

#include <stdexcept>

namespace gui {

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setFixedSize(300,400);

    // Create the button, make "this" the parent
    m_button = new QPushButton("Run", this);
    // set size and location of the button
    m_button->setGeometry(QRect(QPoint(30, 125), QSize(200, 50)));
    m_button->show();
    // Connect button signal to appropriate slot
    connect(m_button, SIGNAL (released()), this, SLOT (handleButton()));

    m_commandOutput = new QTextEdit(this);
    m_commandOutput->setText("Welcome, type 'HELP' for more information.");
    m_commandOutput->setGeometry(QRect(QPoint(30, 25), QSize(200, 70)));
    m_commandOutput->setReadOnly(true);

    m_commandInput = new QLineEdit(this);
    m_commandInput->setGeometry(QRect(QPoint(30, 100), QSize(150, 20)));

    m_submitCommand = new QPushButton(">", this);
    m_submitCommand->setGeometry(QRect(QPoint(185, 99), QSize(45, 22)));
    connect(m_submitCommand, SIGNAL (released()), this, SLOT (handleSendCommand()));
}

void MainWindow::handleButton() {
    if(!mainThread) throw std::runtime_error("mainThread is null");
    m_button->setText(mainThread->toggle() ? "Run" : "Pause");
}

void MainWindow::handleSendCommand()
{
    if(!mainThread) throw std::runtime_error("mainThread is null");

    log(">" + m_commandInput->text());

    QString command = m_commandInput->text().toLower();

    if(command == "exec")
    {

    }
    else if(command == "help")
    {
        log("I still need to do this...");
    }
    else
    {
        log("Unrecognized command: " + command);
    }

    m_commandInput->setText("");
}

}
