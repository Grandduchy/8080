#include <QTemporaryDir>
#include <iostream>
#include "mainwindow.hpp"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    // For now always assume we're running invaders
    loadFile(":/roms/invaders");
}

MainWindow::~MainWindow() {
    delete ui;
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
