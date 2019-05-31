# 8080   
An intel 8080 microprocessor emulation project run with space invaders &nbsp;&nbsp;&nbsp;&nbsp; [![Build Status](https://travis-ci.org/Grandduchy/8080.svg?branch=master)](https://travis-ci.org/Grandduchy/8080)


## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

QT is needed to run space invaders. <br />
Boost libraries are only required to run the tests. <br />
The microprocessor by itself does not require any external libraries other than the C++ standard template library.

#### Installing Prerequisites - QT
Qt can either be installed through Qt's website installer located <a href="https://www.qt.io/download-qt-installer">here</a> or by installing by the command line. <br>
Installing through command line:
```
sudo apt install libqt5webkit5-dev libsqlite3-dev
sudo apt install qtbase5-dev qtdeclarative5-dev
sudo apt install qt5-default qttools5-dev-tools
sudo apt install qtmultimedia5-dev
```
Depending on the installation, Qmake will need to be configured this is done by:
```
export QMAKE=/usr/lib/x86_64-linux-gnu/qt5/bin/qmake
```
#### Installing Prerequisites - Boost Libraries
Boost libraries is installed via the following:
```
sudo apt install libboost-dev
```
### Installing - Getting a development environment running
Either download and unzip this repository via the top right or by cloning the repository first:
```
git clone https://github.com/Grandduchy/8080
cd 8080
```
Use Qmake and Make to create the executable
```
qmake
make
```
Depending on the configuration, add executable permissions to the executable 8080, and run
```
chmod +x 8080
./8080
```

## Running the tests

### General Tests
The general tests have two components an opcode test and CPU exercisers <br>
Opcode Tests are tests for a specific opcode or family of opcodes(MOV A,A to MOV L,L) <br>
CPU exercisers are programs compiled via an i8080 Assembler acting as a test written by third party to be run on the cpu<br>
The CPU exercisers are available in source and compiled in 8080/rsc <br><br>
After cloning to 8080/test, There are two ways to run the general tests <br>
Option 1: Run the python file in the folder before making the tests<br>
```
allowRunTests.py
```
Option 2: Modify 8080/include/tester.h, uncomment the two lines:
```
//#define TESTENABLE -> #define TESTENABLE
//#define TESTS -> #define TESTS
```
Afterwards run the following to run the tests:
```
qmake && make
./runTest
```

### Manual Testing
Manual testing involves inputting a specific amount of times for the cpu to run and displays the state of the registers, program counter, stack pointer, etc. <br>
To run Modify 8080/include/tester.h by uncommenting the following:
```
//#define TESTENABLE -> #define TESTENABLE
//#define MANUALTEST -> #define MANUALTEST
```
Manual testing then are used via 8080/test/manualtest.cpp
```
g++ -std=c++14 manualtest.cpp -L .....
```

## Built With

* [QT](https://doc.qt.io/) - The GUI framework
* [Boost](https://www.boost.org/) - C++ Libraries
* [Travis-CI](https://travis-ci.org/) - Continuous Integration Framework

## Authors

* **Joshua Challenger** - *Complete Project* - [Grandduchy](https://github.com/Grandduchy)

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments and Sources

* [Emulator101](http://www.emulator101.com) 
* [Emutalk](http://www.emutalk.net/threads/38177-Space-Invaders) helpful discussion threads, used for memory maps and I/O with ports
* [Cpu Exercisors](http://www.emulator101.com)
* [Superzazu's 8080](https://github.com/superzazu/8080) for understanding how to deal with aux flag
* [8080 opcode Table](http://www.pastraiser.com/cpu/i8080/i8080_opcodes.html)
* [suanyuan's Space Invaders](http://www.emutalk.net/threads/38177-Space-Invaders/page7) for understanding in interrupts
