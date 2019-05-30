#include <QTemporaryDir>
#include <QKeyEvent>
#include <QPainter>
#include <iostream>
#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QPoint>

int MainWindow::reFac = 2;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    QSize sz(256 * reFac, 256 * reFac);
    debugWindow = new DebugWindow();
    //debugWindow->show();
    ui->centralWidget->setFixedSize(sz);
    ui->centralWidget->setMinimumSize(sz);
    ui->centralWidget->setMaximumSize(sz);
    // Every time timer goes off, runcycle will run
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::runCycle);
    timer->start(0);
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

void MainWindow::OP_Input() {
    uint8_t portNum = state.memory[state.programCounter + 1];
    state.programCounter += 2;
    switch(portNum) {
        case 0:
            state.a = 0xF;
            break;
        case 1:
            state.a = state.port1;
            break;
        case 2:
            state.a = state.port2;
            break;
        case 3:  {// Returns the result of the shift into accumulator
            uint16_t fullByte = static_cast<uint16_t>((static_cast<uint16_t>(state.shiftLHS) << 8) | state.shiftRHS);
            uint8_t result = static_cast<uint8_t>(fullByte >> (8 - state.shiftOffset) );
            state.a = result;
        }
        break;
        default:
            std::cerr << "Possible Input Error at 0x" << std::hex << state.programCounter << ", port=" << static_cast<int>(portNum) << std::endl;
    }
}

void MainWindow::OP_Output(const uint8_t& value) {
    uint8_t port = state.memory[state.programCounter + 1];
    state.programCounter += 2;
    switch(port) {
        case 1:
            state.port1 = value;
            break;
        case 2:
            state.shiftOffset = value & 0x7;
            break;
        case 3:
            state.port3 = value;
            // A sound effect would go in here.
            break;
        case 4: // value acts as the new lhs
            state.shiftRHS = state.shiftLHS;
            state.shiftLHS = value;
            break;
        case 5:
            state.port5 = value;
            // A sound effect would go in here.
            break;
    }
}

void MainWindow::runCycle() {
    static uint8_t interruptNum = 1;

    // this time is in miliseconds
    static constexpr uint8_t next60Hz = 120 / 9;
    static int64_t nextInterrupt = 0;
    static int64_t nextDraw = 0;

    if (nextInterrupt == 0) {
        nextInterrupt = getTime() + next60Hz;
        nextDraw = getTime() + next60Hz;
    }

    if (state.allowInterrupt && getTime() >= nextInterrupt) {
        cpu.generateInterrupt(state, interruptNum);
        interruptNum =  interruptNum == 1 ? 2 : 1;
        nextInterrupt = getTime() + next60Hz;
    }

    uint8_t opcode = state.memory[state.programCounter];
    // For now emulation will occur in two places
    // Input, next byte is read from input device number and replaces accumulator
    if (opcode == 0xDB) {
        OP_Input();
    }
    // Output
    // The contents of accumulator are sent to output device number
    else if (opcode == 0xD3) {
        OP_Output(state.a);
    }
    else {
        cpu.runCycle(state);
    }
    if (getTime() >= nextDraw) {
        repaint();
        nextDraw = getTime() + next60Hz;
    }
    ++cpuSteps;
}

void MainWindow::paint() {
    static constexpr int yOffset = 40;
    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);
    painter.drawRect(0,0, 256 * reFac, 256 * reFac + yOffset);
    painter.setPen(Qt::white);
    painter.setBrush(Qt::white);
    auto& memory = state.memory;

    painter.rotate(-90);

    for (int y = 0; y != 224; y++) {
        for (int x = 0; x != 256; x += 8) {
            // VRAM starts at 0x2400, y and x expressions are offset
            std::size_t vRamLoc = static_cast<std::size_t>(0x2400 + (y * 32) + (x / 8));
            uint8_t byte = memory[vRamLoc];
            for (int shift = 0; shift != 8; shift++) {
                bool bit = (byte >> shift) & 1;
                if (bit) {
                    // The - 60 is made from observation
                   // painter.drawRect((x+shift) * 2, y * 2 + yOffset, 2, 2); <- original drawRect without rotation
                    painter.drawRect( (x+shift) * reFac - (height * reFac) - yOffset - 60, y * reFac + yOffset, reFac, reFac);
                }
            }

        }
    }
}

inline int64_t MainWindow::getTime() const noexcept {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
           ).count();
}

void MainWindow::paintEvent(QPaintEvent*) {
    paint();
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
