#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>

namespace gui {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
private slots:
    void handleButton();
private:
    QPushButton *m_button;
    bool state = true;
signals:

public slots:
};

}

#endif // MAINWINDOW_H
