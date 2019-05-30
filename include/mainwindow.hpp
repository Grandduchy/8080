#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QMap>
#include <QTimer>
#include "Disassembler8080.hpp"
#include "State8080.hpp"
#include "debugwindow.hpp"


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
    void paintEvent(QPaintEvent* ) override;
private:
    Ui::MainWindow * ui;

    State8080 state;
    Disassembler8080 cpu;
    QMap<Qt::Key, bool> keyMap;
    QTimer* timer;
    DebugWindow* debugWindow;

    uint32_t cpuSteps = 0;
    static constexpr int width = 256;
    static constexpr int height = 224;
    static int reFac; // resize factor, 1 for 256x224...
    void loadFile(const QString& qtRscFile);
    void setKey(QKeyEvent*& key, bool toggle);
    void runCycle();
    void paint();

    void OP_Input();
    void OP_Output(const uint8_t& value);

    inline int64_t getTime() const noexcept;
};

#endif // MAINWINDOW_HPP
