
#include "tester.h"

#ifndef TESTENABLE

#include "mainwindow.hpp"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a (argc, argv);
    MainWindow window;
    window.show();

    return a.exec();
}


#endif
