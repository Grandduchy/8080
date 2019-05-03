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
    {// OP_DAD H 0x29
    // make sure this doubles the value of h.
        state.clearAll();
        memory[0] = 0x29;
        state.h = 0x06;
        state.l = 0xA6;
        dis.runCycle(state);
        bool passed = state.h == 0x0D && state.l == 0x4C;
        if (!passed)
                BOOST_ERROR("0x29 OP_DAD H failure");
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
    {// OP LXI SP D16 0x31
        state.clearAll();
        memory[0] = 0x31;
        memory[1]= 0x25;
        memory[2] = 0xAF;
        dis.runCycle(state);
        bool passed = state.stackPointer == 0x25AF;
        if (!passed)
            BOOST_ERROR("0x31 LXI SP failure");
    }
    {// OP MVIM D8 0x36
        state.clearAll();
        state.h = 0xAF;
        state.l = 0xC4;
        memory[0] = 0x36;
        memory[1] = 0xAA;
        memory[0xAFC4] = 0xFF;
        dis.runCycle(state);
        if(memory[0xAFC4] != 0xAA)
            BOOST_ERROR("0x36 MVIM D8 failure");


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
    { // OP_STA ADR 0x32
        state.clearAll();
        state.a = 0xFF;
        memory[0] = 0x32;
        memory[1] = 0xA7;
        memory[2] = 0xE2;
        dis.runCycle(state);
        if (memory[0xE2A7] != state.a)
            BOOST_ERROR("0x32 OP_STA ADR failure");
    }
    { // OP_LDA ADR 0x3A
        state.clearAll();
        state.a = 0xF0;
        memory[0] = 0x3A;
        memory[1] = 0x4A;
        memory[2] = 0xCC;
        memory[0xCC4A] = 0xA2;
        dis.runCycle(state);
        if (state.a != 0xA2)
            BOOST_ERROR("0xA2 OP LDA ADR failure");

    }
    { // MOV tests
        state.clearAll();
        // test special 0x77
        memory[0] = 0x77;
        state.h = 0x2B;
        state.l = 0xE9;
        state.a = 0xAF;
        memory[0x2BE9] = 0xFF;
        dis.runCycle(state);
        if (memory[0x2BE9] != state.a)
            BOOST_ERROR("0x77 OP MOV M,A failure");
        // test 0x56
        memory[1] = 0x56;
        memory[0x2BE9] = 0x0A;
        dis.runCycle(state);
        if (state.d != 0x0A)
            BOOST_ERROR("0x56 OP MOV D,M failure");
        // test 0x6F
        memory[2] = 0x6F;
        state.l = 0xEB;
        dis.runCycle(state);
        if (state.l != state.a)
            BOOST_ERROR("0x6F OP MOV L,A failure");
    }

}
