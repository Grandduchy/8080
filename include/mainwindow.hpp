#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include "Disassembler8080.hpp"
#include "State8080.hpp"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow * ui;

    State8080 state;
    Disassembler8080 cpu;

    void loadFile(const QString& qtRscFile);
};

#endif // MAINWINDOW_HPP
