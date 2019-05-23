#include "tester.h"

#ifdef CPUDIAGTEST

#include <iostream>
#include <algorithm>

#include "Disassembler8080.hpp"
#include "State8080.hpp"

void runTest(const std::string& filename) {

    State8080 state = stateFromFile(filename, 0x100);

    auto slash = std::find(filename.rbegin(), filename.rend(), '/').base();
    std::string file(slash, filename.end());

    std::cout << "**** Test running " << file << "\n";

    // 8080pre test will fail regardless of correctness if the stack pointer is not set
    if (file == "8080PRE.COM")
        state.stackPointer = 0xFFF5;


    // Set PC to start where ROM starts
    state.programCounter = 0x100;

    // Make 0x5 a RET to return back from a call to 0x5
    state.memory[0x5] = 0xC9;
    Disassembler8080 dis;
    for (std::size_t count = 0 ;; count++) {

        // call to location 5 if the tests require BDOS to print
        if (state.programCounter == 0x5) {
            // BDOS wants to print characters stored in memory @ DE, until the character $ in ASCII is found
            // Character $ is 0x24 in hex
            if (state.c == 9) {
                uint16_t DE = static_cast<uint16_t>((static_cast<uint16_t>(state.d) << 8) | state.e);
                for (uint16_t loc = DE; state.memory[loc] != 0x24; loc++) {
                    std::cout << static_cast<char>(state.memory[loc]);
                }
            }
            // BDOS wants to print a single character in register E
            else if (state.c == 2) {
                std::cout << static_cast<char>(state.e);
            }
        }

        dis.runCycle(state);

        // call to location 0 if the test is done
        if (state.programCounter == 0) {
            std::cout << "\n";
            break;
        }
    }
}


int main() {
    runTest("../8080/rsc/8080PRE.COM");
    runTest("../8080/rsc/TST8080.COM");
    std::cout << std::endl;

    return 0;
}



#endif
