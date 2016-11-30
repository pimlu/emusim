#include "mainwindow.h"

#include <stdexcept>

namespace gui {

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setFixedSize(300,400);

    m_button = new QPushButton("Run", this);
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
    m_button->setText(mainThread->toggle() ? "Run" : "Pause");
}

void MainWindow::handleSendCommand()
{
    if(!mainThread) throw std::runtime_error("mainThread is null");

    log(">" + m_commandInput->text());

    QString command = m_commandInput->text().toLower();

    if(command == "exec")
    {
        log("I still need to do this...");
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
