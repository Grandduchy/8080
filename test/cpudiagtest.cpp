#include "tester.h"

#if defined(CPUDIAGTEST) && !defined(CPUDIAG)

#define CPUDIAG

#include <iostream>
#include <thread>

#include "Disassembler8080.hpp"
#include "State8080.hpp"


int main() {
    State8080 state = stateFromFile("../8080/rsc/TST8080.COM", 0x100);

    // Fix the first instruction to jump to 0x100
    state.memory[0] = 0xc3;
    state.memory[1] = 0;
    state.memory[2] = 0x01;

    /*
    // skip DAA test
    state.memory[0x59c] = 0xc3; // JMP
    state.memory[0x59d] = 0xc2;
    state.memory[0x59e] = 0x05;

    */
    Disassembler8080 dis;
    // i == 12, i == 31 - 36
    for (int i = 0; i != 1000 ;i++) {
        dis.runCycle(state);
    }
    return 0;
}



#endif
