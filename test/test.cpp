#define BOOST_TEST_MODULE opcode_test
#include <boost/test/included/unit_test.hpp>
#include "State8080.hpp"
#include "Disassembler8080.hpp"

BOOST_AUTO_TEST_CASE( arithmetric_tests ) {
    State8080 state;
    std::array<uint8_t, State8080::RAM>& memory = state.memory;
    Disassembler8080 dis;

    {// OP_LXIB_D16 test using 0x01, if this test succeeds then it is safe to assume LXID_D16 and other
        // instructions are also safe as it uses the same code.

        memory[0] = 0x01;
        memory[1] = 0x50;
        memory[2] = 0x80;
        dis.runCycle(state);
        if (!(state.b == 0x80 && state.c == 0x50)) {
            std::string s("0x01 OP LIXB->D16 failure in b=" + std::to_string(state.b) + ", c=" + std::to_string(state.c));
            BOOST_ERROR(s);
        }
    }
    { // OP_DCR B 0x05
        state.clearSpecial();
        memory[0] = 0x05;
        // check if it decriments a number
        state.b = 0xF;
        dis.runCycle(state);
        if (state.b != 0xE){
            std::string s("0x05 OP_DCRB failure, b=" + std::to_string(state.b));
            BOOST_ERROR(s);
        }
        state.clearSpecial();
        // check if zero is set
        state.b = 0x1;
        dis.runCycle(state);
        if (state.condFlags.zero == 0)
            BOOST_ERROR("0x05 OP_DCRB failure, zero not set");
        state.clearSpecial();
        // check if sign is affected
        state.b = 0xFF;
        dis.runCycle(state);
        if (state.condFlags.sign == 0)
            BOOST_ERROR("0x05 OP_DCRB failure, sign not set");

    }
    { // OP_MVIB_D8 0x06
        state.clearAll();
        memory[0] = 0x06;
        memory[1] = 0xF8;
        dis.runCycle(state);
        if (state.b != 0xF8)
            BOOST_ERROR("0x06 OP_MVIB_D8 failure, b=" + std::to_string(state.b));
    }
    {// OP_DAD B // 0x09
        state.clearAll();
        memory[0] = 0x09;
        state.b = 0x33;
        state.c = 0x9F;
        state.h = 0xA1;
        state.l = 0x7B;
        dis.runCycle(state);
        bool passed = state.h == 0xD5 && state.l == 0x1A && state.condFlags.carry == 0;
        if (!passed)
            BOOST_ERROR("0x09 OP_DAD B failure");
    }
    {// OP_INXD 0x13
        state.clearAll();
        memory[0] = 0x13;
        state.d = 0x38;
        state.e = 0xff;
        dis.runCycle(state);
        bool passed = state.d == 0x39 && state.e == 0x00;
        if (!passed)
            BOOST_ERROR("0x13 INX D failure");
    }

}

BOOST_AUTO_TEST_CASE( logical_tests) {
    State8080 state;
    std::array<uint8_t, State8080::RAM>& memory = state.memory;
    Disassembler8080 dis;

    {// OP_RRC 0x0f
        state.clearAll();
        memory[0] = 0x0f;
        state.a = 0xF3;
        dis.runCycle(state);
        bool passed = state.a == 0xF9 && state.condFlags.carry == 1;
        if (!passed)
            BOOST_ERROR("0x0f OP_RRC failure a=" + std::to_string(state.a));
    }

}

BOOST_AUTO_TEST_CASE( other_tests) {
    State8080 state;
    std::array<uint8_t, State8080::RAM>& memory = state.memory;
    Disassembler8080 dis;

    {// OP_LDAX D 0x1a
        state.clearAll();
        memory[0] = 0x1a;
        memory[0x938B] = 0xFF;
        state.d = 0x93;
        state.e = 0x8B;
        dis.runCycle(state);
        if (state.a != 0xFF)
            BOOST_ERROR("0x1a OP_LDAX D failure, a=" + std::to_string(state.a));
    }


}
