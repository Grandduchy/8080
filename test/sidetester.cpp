#include "tester.h"

#ifdef sideTest
#include <iostream>
#include <string>
#include <iomanip>
#include <functional>

#include "State8080.hpp"
#include "Disassembler8080.hpp"
#include "ConditionFlags.hpp"

template<typename RT>
RT check(const std::string& error_message) {
    //checking inputs recieved, if the stream is good and if they match what characters were wanted.
    RT x;
    while (std::cin.good()) {
        std::cin >> x;
        if (std::cin.fail()) {
            if (!error_message.empty())
                std::cout << error_message << std::endl;
            std::cin.clear();
            std::cin.ignore();
            continue;
        }
        else {//cin is in a good condition
            return x;
        }

    }//while
    throw std::runtime_error("Did not recieve input from user");
    return RT();// return empty object if function fails
}

std::string flags(const State8080& state) {
    std::string flag;
    if (state.condFlags.zero) flag.push_back('z');
    else flag.push_back('.');

    if (state.condFlags.sign) flag.push_back('s');
    else flag.push_back('.');

    if (state.condFlags.parity) flag.push_back('p');
    else flag.push_back('.');

    if (state.allowInterrupt) flag.push_back('i');
    else flag.push_back('.');

    if (state.condFlags.carry) flag.push_back('c');
    else flag.push_back('.');

    return flag;
}

int main() {
    State8080 state;
    std::cout << "Enter the amount of cycles to run, -1 is to restart, -2 is to exit" << std::endl;
    try {
        state = stateFromFile("../8080/rsc/invaders");
    } catch (...) {
        std::cerr << "Failed write into memory, problems may ensure. \n";
    }

    Disassembler8080 dis;
    std::cout << std::hex;
    int sum = 0;
    while(true) {
        int i = check<int>("");
        if (i == -2) break;
        if (i == -1) {
            std::cout << "Restarting... \n";
            sum = 0;
            state = stateFromFile("../8080/rsc/invaders");
            continue;
        }
        // 37411
        // 37413
        for (int times = 0; times != i; times++) {
            dis.runCycle(state);
            ++sum;
            if (dis.wasUnimplemented) {
                std::cout << "Unimplemented, possible Error at: " << sum << std::endl;
                dis.wasUnimplemented = false;
            }
            if (dis.wasTodo) {
                std::cout << "Noted as todo at:" << sum << std::endl;
                dis.wasTodo = false;
            }
        }


        std::cout << "a\tb\tc\td\te\th\tl\tpc\tsp\tflags\ttot\n" << std::hex;
        // C style cast is bad but ok here.
        std::cout << (int)state.a << "\t" << (int)state.b << "\t" << (int)state.c << "\t" << (int)state.d
                << "\t" << (int) state.e << "\t" << (int)state.h << "\t" << (int)state.l << "\t"
                << (int)state.programCounter << "\t" <<(int)state.stackPointer << "\t" << flags(state)
                << "\t" << std::dec << sum << std::hex << "\n";
    }
    return 0;
}


#endif
