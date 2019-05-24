#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QMap>
#include <QTimer>
#include "Disassembler8080.hpp"
#include "State8080.hpp"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void keyPressEvent(QKeyEvent* key) override;
    void keyReleaseEvent(QKeyEvent* key) override;
private:
    Ui::MainWindow * ui;

    State8080 state;
    Disassembler8080 cpu;
    QMap<Qt::Key, bool> keyMap;
    QTimer* timer;

    void loadFile(const QString& qtRscFile);
    void setKey(QKeyEvent*& key, bool toggle);
    void runCycle();

    void OPInput();
    void OPOutput();
};

#endif // MAINWINDOW_HPP
