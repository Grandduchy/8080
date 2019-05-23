#include "tester.h"

#if defined(CPUDIAGTEST) && !defined(CPUDIAG)

#define CPUDIAG

#include <iostream>
#include <thread>

#include "Disassembler8080.hpp"
#include "State8080.hpp"


int main() {
    State8080 state = stateFromFile("../8080/rsc/TST8080.COM", 0x100);

    // Set PC to start where ROM starts
    state.programCounter = 0x100;

    // Make 0x5 a RET to return back from a call to 0x5
    state.memory[0x5] = 0xC9;


    Disassembler8080 dis;
    uint16_t posErr = 0;
    for (std::size_t count = 0; count != 1000; count++) {

        // 6A0 is where CPUER subroutine is located and indicates an error happened
        if (state.programCounter == 0x6A0) {
            posErr = static_cast<uint16_t>(count) - 2;
        }

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
            if (posErr != 0)
                std::cout << "\nEnding at iteration (dec): " << std::dec << count << "\nPossible Error at (dec) : " << posErr;
            std::cout << std::endl;
            break;
        }
    }
    return 0;
}



#endif
