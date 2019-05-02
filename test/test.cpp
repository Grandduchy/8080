#define BOOST_TEST_MODULE opcode_test
#include <boost/test/included/unit_test.hpp>
#include "State8080.hpp"
#include "Disassembler8080.hpp"

BOOST_AUTO_TEST_CASE( opcode_tests ) {
    State8080 state;
    std::array<uint8_t, State8080::RAM>& memory = state.memory;

    // OP_LXIB_D16 test using 0x01
    memory[0] = 0x01;
    memory[1] = 0x50;
    memory[2] = 0x80;
    Disassembler8080 dis;
    dis.runCycle(state);
    if (!(state.b == 0x80 && state.c == 0x50)) {
        std::string a("0x01 OP LIXB->D16 failure in b=" + std::to_string(state.b) + ", c=" + std::to_string(state.c));
        BOOST_ERROR(a);
    }

}
