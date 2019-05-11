
#define TEST_ENABLE

#ifdef TEST_ENABLE
#define BOOST_TEST_MODULE exercisertest
#include <boost/test/included/unit_test.hpp>

#include "Disassembler8080.hpp"
#include "State8080.hpp"

BOOST_AUTO_TEST_CASE( exerciser ) {
    State8080 state = stateFromFile("../8080/rsc/cpudiag", 0x100); // .. for now
    // Given from emulator101
    //Fix the stack pointer from 0x6ad to 0x7ad
    // this 0x06 byte 112 in the code, which is
    // byte 112 + 0x100 = 368 in memory
    state.memory[368] = 0x7;

    //Skip/jump over the DAA test
    state.memory[0x59c] = 0xc3; //JMP
    state.memory[0x59d] = 0xc2;
    state.memory[0x59e] = 0x05;
    Disassembler8080 diassembler;
    while(true) {
        diassembler.runCycle(state);
    }

}

#endif
