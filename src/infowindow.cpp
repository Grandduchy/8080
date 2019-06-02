#include "include/infowindow.hpp"
#include "ui_infowindow.h"

InfoWindow::InfoWindow(QWidget *parent) :
    QWidget(parent),  ui(new Ui::InfoWindow) {

    ui->setupUi(this);
    this->setWindowTitle("Information");
}

InfoWindow::~InfoWindow() {
    delete ui;
}
