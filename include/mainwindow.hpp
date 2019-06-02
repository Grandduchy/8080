#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QMap>
#include <QTimer>
#include <QMediaPlayer>
#include <QSoundEffect>
#include "Disassembler8080.hpp"
#include "State8080.hpp"
#include "infowindow.hpp"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow() override;

protected:
    void virtual keyPressEvent(QKeyEvent* key) override;
    void virtual keyReleaseEvent(QKeyEvent* key) override;
    void virtual paintEvent(QPaintEvent* ) override;
private:
    Ui::MainWindow * ui;
    InfoWindow * infoWindow;
    State8080 state;
    Disassembler8080 cpu;
    QMap<Qt::Key, bool> keyMap; // key map to determine if a specific key is on
    QTimer* timer; // timer used to loop running cycles

    uint32_t cpuSteps = 0; // the amount of times cpu has been executed
    static constexpr int width = 256;
    static constexpr int height = 224;
    static int reFac; // resize factor, 1 for 256x224...
    void loadFile(const QString& qtRscFile);
    void setKey(QKeyEvent*& key, bool toggle);
    void runCycle();
    void paint();

    void registerInput();
    void soundHandle();
    void OP_Input();
    void OP_Output(const uint8_t& value);

    inline int64_t getTime() const noexcept;
    QMediaPlayer* player; // sound player
    QSoundEffect ufoSoundEffect;
};

#endif // MAINWINDOW_HPP
