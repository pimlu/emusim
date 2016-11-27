#include "mainwindow.h"

#include <stdexcept>

namespace gui {

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setFixedSize(300,400);
    // Create the button, make "this" the parent
    m_button = new QPushButton("Run", this);
    // set size and location of the button
    m_button->setGeometry(QRect(QPoint(30, 100), QSize(200, 50)));
    m_button->show();
    // Connect button signal to appropriate slot
    connect(m_button, SIGNAL (released()), this, SLOT (handleButton()));
}

void MainWindow::handleButton() {
    if(!mainThread) throw std::runtime_error("mainThread is null");
    m_button->setText(mainThread->toggle() ? "Run" : "Pause");
}

}
