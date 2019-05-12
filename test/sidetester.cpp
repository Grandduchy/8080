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

std::string flags(ConditionFlags flags) {
    std::string flag;
    if (flags.zero) flag.push_back('z');
    else flag.push_back('.');

    if (flags.sign) flag.push_back('s');
    else flag.push_back('.');

    if (flags.parity) flag.push_back('p');
    else flag.push_back('.');

    if (flags.carry) flag.push_back('c');
    else flag.push_back('c');

    return flag;
}

int main() {
    State8080 state;
    state = stateFromFile("../8080/rsc/invaders", 0);
    Disassembler8080 dis;
    std::cout << std::hex;
    int sum = 0;
    while(true) {
        int i = check<int>("");
        if (i == -1) break;

        for (int times = 0; times != i; times++) {
            dis.runCycle(state);
            ++sum;
        }
        std::cout << "a\tb\tc\td\te\th\tl\tpc\tsp\tflags\ttot\n";
        // C style cast is bad but ok here.
        std::cout << (int)state.a << "\t" << (int)state.b << "\t" << (int)state.c << "\t" << (int)state.d
                << "\t" << (int) state.e << "\t" << (int)state.h << "\t" << (int)state.l << "\t"
                << (int)state.programCounter << "\t" <<(int)state.stackPointer << "\t" << flags(state.condFlags)
                << "\t" << std::dec << sum << std::hex << "\n";
    }
    return 0;
}


#endif
