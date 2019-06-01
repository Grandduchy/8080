#include <QTemporaryDir>
#include <QKeyEvent>
#include <QPainter>
#include <QMediaPlayer>
#include <QSoundEffect>
#include <QSound>
#include <QUrl>
#include <iostream>
#include <chrono>
#include "mainwindow.hpp"
#include "ui_mainwindow.h"



int MainWindow::reFac = 2; // The resize factor

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

    loadFile(":/roms/rsc/invaders");
    player = new QMediaPlayer(this);

    ufoSoundEffect.setSource(QUrl("qrc:/media/rsc/audio/Ufo.wav"));
    ufoSoundEffect.setVolume(0.33);
    ufoSoundEffect.setLoopCount(QSoundEffect::Infinite);
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
    auto registerKey = [&](const Qt::Key& key_t){
      if (key->key() == key_t)
          keyMap[key_t] = toggle;
    };
    registerKey(Qt::Key_C); // coin key
    // player 1 keys
    registerKey(Qt::Key_W);
    registerKey(Qt::Key_A);
    registerKey(Qt::Key_D);
    registerKey(Qt::Key_S);
    // player 2 keys
    registerKey(Qt::Key_I);
    registerKey(Qt::Key_J);
    registerKey(Qt::Key_L);
    registerKey(Qt::Key_K);

}
void MainWindow::registerInput() {
    auto setKeyToPort = [&](uint8_t& port, const uint8_t& bit, const Qt::Key& key) {
        // Ensure key exists
        if (keyMap.find(key) == keyMap.end())
            return;
        // if the key is on set the bit, otherwise clear the bit
        if (keyMap[key]) {
            port |= (1 << bit);
        }
        else {
            port &= ~(1 << bit);
        }
    };

    setKeyToPort(state.port1, 0, Qt::Key_C); // coin
    setKeyToPort(state.port1, 2, Qt::Key_S); // p1 start
    setKeyToPort(state.port1, 4, Qt::Key_W); // shoot
    setKeyToPort(state.port1, 5, Qt::Key_A); // p1 joystick left
    setKeyToPort(state.port1, 6, Qt::Key_D); // p1 jotstick right

    setKeyToPort(state.port1, 1, Qt::Key_K); // p2 start
    setKeyToPort(state.port2, 4, Qt::Key_I); // p2 shoot
    setKeyToPort(state.port2, 5, Qt::Key_J); // p2 joystick left
    setKeyToPort(state.port2, 6, Qt::Key_L); // p2 joystick right

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
            soundHandle();
            // A sound effect would go in here.
            break;
        case 4: // value acts as the new lhs
            state.shiftRHS = state.shiftLHS;
            state.shiftLHS = value;
            break;
        case 5:
            state.port5 = value;
            // A sound effect would go in here.
            soundHandle();
            break;
    }
}



void MainWindow::soundHandle() {
    static uint8_t lastPort3 = 0;
    static uint8_t lastPort5 = 0;

    // Function plays a sound if the condition is met with both state ports
    static auto playSoundIf = [&](const uint8_t& statePort, const uint8_t& lastPort, const uint8_t& expr, const QString& soundFName){
        if ( (statePort & expr) && !(lastPort & expr) ) {
            std::string f = soundFName.toStdString();
            auto slash = std::find(f.rbegin(), f.rend(), '/').base();
            std::string walk(slash, slash + 4);
            // If the sound is one of the walks, QSound cannot play it, use qmediaplayer instead
            if (walk == "Walk") {
                player->setMedia(QUrl(soundFName));
                player->play();
            }
            else
                QSound::play(soundFName);
        }
    };

    static const QString pathPrefix("qrc:/media/rsc/audio/");

    static auto soundPlayP3 = [&](const uint8_t& expr, const QString& soundFName) {
      playSoundIf(state.port3, lastPort3, expr, pathPrefix + soundFName);
    };
    static auto soundPlayP5 = [&](const uint8_t& expr, const QString& soundFName) {
      playSoundIf(state.port5, lastPort5, expr, pathPrefix + soundFName);
    };


    if (lastPort3 != state.port3) {
        // UFO sounds, they constantly loop, all other sounds are different in that they only play once
        if ( (state.port3 & 0x1) && !(lastPort3 & 0x1) ) {
            ufoSoundEffect.play();
            std::cerr << "Start\n";
        }
        else if ( !(state.port3 & 0x1) && (lastPort3 & 0x1) ) {
            ufoSoundEffect.stop();
            std::cerr << "Stop\n";
        }
        soundPlayP3(0x2, "Shot.wav");
        soundPlayP3(0x4, "BaseHit.wav");
        soundPlayP3(0x8, "InvHit.wav");
        lastPort3 = state.port3;
    }


    if (lastPort5 != state.port5) {
        soundPlayP5(0x1, "Walk1.wav");
        soundPlayP5(0x2, "Walk2.wav");
        soundPlayP5(0x4, "Walk3.wav");
        soundPlayP5(0x8, "Walk4.wav");
        soundPlayP5(0x10, "UfoHit.wav");
        lastPort5 = state.port5;
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
        registerInput();
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
