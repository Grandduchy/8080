#include <QTemporaryDir>
#include <QKeyEvent>
#include <iostream>
#include "mainwindow.hpp"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    // Every time timer goes off, runcycle will run
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::runCycle);
    timer->start(1000);
    // For now always assume we're running invaders
    loadFile(":/roms/invaders");
}

MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::keyPressEvent(QKeyEvent* key) {
    setKey(key, true);
}

void MainWindow::keyReleaseEvent(QKeyEvent* key) {
    setKey(key, false);
}

void MainWindow::setKey(QKeyEvent*& key, bool toggle) {
    if (key->key() == Qt::Key_Right)
        keyMap[Qt::Key_Right] = toggle;
    if (key->key() == Qt::Key_Left)
        keyMap[Qt::Key_Left] = toggle;
}

void MainWindow::OPInput() {
    static std::array<uint16_t, 8> portOut{};
    uint8_t portNum = state.memory[state.programCounter + 1];
    state.programCounter += 2;
    switch(portNum) {
        case 3:  {// bits 0-7 shift register data

        }
        break;
        default:
            std::cerr << "Possible Error at 0x" << std::hex << state.programCounter << std::endl;
    }
}
void MainWindow::OPOutput() {
    static std::array<uint16_t, 8> portIn{};

}

void MainWindow::runCycle() {
    uint8_t opcode = state.memory[state.programCounter];
    // For now emulation will occur in two places

    // Input, next byte is read from input device number and replaces accumulator
    if (opcode == 0xDB) {
        OPInput();
    }
    // Output
    // The contents of accumulator are sent to output device number
    else if (opcode == 0xD3) {
        OPOutput();
    }
    else {
        cpu.runCycle(state);
    }
}

void MainWindow::loadFile(const QString& qtRscFile) {
    try {
        QTemporaryDir tempDir;
        QString tempFile;
        if (tempDir.isValid())
            tempFile = tempDir.path() + "/TempRom";
        else
            throw std::runtime_error("Temporary directory cannot be created");

        if (!QFile::copy(qtRscFile, tempFile))
            throw std::runtime_error("Unable to copy resource file into temporary");

        if (!QFileInfo::exists(tempFile))
            throw std::runtime_error("Unable to copy resource file to storage");
        state = stateFromFile(tempFile.toStdString(), 0);
    } catch (std::exception& e) {
        std::cerr << "Error loading rom into emulator, " << e.what() << std::endl;
    }
}
