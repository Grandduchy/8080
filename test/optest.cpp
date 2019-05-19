#include "tester.h"

#ifdef OPTEST
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
        bool passed = state.stackPointer == 0xAF25;
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
    {// OP ADI D8 0xC6
        state.clearAll();
        state.a = 0x56;
        memory[0] = 0xC6;
        memory[1] = 0xBE;
        dis.runCycle(state);
        bool passed = state.a == 0x14 && state.condFlags.carry == state.condFlags.parity && state.condFlags.carry == 1
                && state.condFlags.zero == 0 && state.condFlags.sign == 0;
        if (!passed)
            BOOST_ERROR("0xC6 ADI D8 failure");
    }
    {// OP ANI D8 0xE6
        state.clearAll();
        memory[0] = 0xE6;
        state.a = 0x3A;
        memory[1] = 0x0F;
        dis.runCycle(state);
        if (state.a != 0x0A)
            BOOST_ERROR("0xE6 ANI D8 failure");
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
    { // OP RLC 0x07
        state.clearAll();
        memory[0] = 0x07;
        state.a = 0xF2;
        state.condFlags.carry = 0;
        dis.runCycle(state);
        bool passed = state.a == 0xE5 && state.condFlags.carry == 1;
        if (!passed)
            BOOST_ERROR("OP RLC 0x07 failure");
    }
    { // OP RAL 0x17
        state.clearAll();
        state.condFlags.carry = 0;
        state.a = 0xB5;
        memory[0] = 0x17;
        dis.runCycle(state);
        bool passed = state.a == 0x6A && state.condFlags.carry == 1;
        if (!passed)
            BOOST_ERROR("OP RAL 0x17 failure");
    }
    { // OP RAR 0x1F
        state.clearAll();
        state.a = 0x6A;
        memory[0] = 0x1F;
        state.condFlags.carry = 1;
        dis.runCycle(state);
        bool passed = state.a == 0xB5 && state.condFlags.carry == 0;
        if (!passed)
            BOOST_ERROR("OP RAR 0x1F failure");
    }
    {// OP_ANAA 0xA7
        state.clearAll();
        memory[0] = 0xA7;
        state.a = 0xFA;
        dis.runCycle(state);
        if (state.a != 0xFA && state.condFlags.carry != 0)
            BOOST_ERROR("0xA7 OP ANA A failure");
    }
    {// OP_XRAA 0xAF
        state.clearAll();
        memory[0] = 0xAF;
        state.a = 0xB9;
        dis.runCycle(state);
        if (state.a != 0 && state.condFlags.carry != 0)
            BOOST_ERROR("0xAF OP XRA A failure");
    }
    {// OP _ADD D test
        state.clearAll();
        memory[0] = 0x82;
        state.d = 0x2E;
        state.a = 0x6C;
        dis.runCycle(state);
        bool passed = state.a == 0x9a && state.condFlags.zero == 0 && state.condFlags.carry == 0 &&
                state.condFlags.parity == 1 && state.condFlags.sign == 1 && state.condFlags.auxCarry == 1;
        if (!passed)
            BOOST_ERROR("ADD failure");
        memory[1] = 0x87;
        state.a = 0x2E;
        dis.runCycle(state);
        if (state.a != 2 * 0x2E)
            BOOST_ERROR("ADD failure of doubling");
    }
    { // OP ADC C test
        state.clearAll();
        state.condFlags.carry = 0;
        state.a = 0x42;
        state.c = 0x3D;
        memory[0] = memory[1] = 0x89;

        dis.runCycle(state);
        bool passed = state.a == 0x7F && state.condFlags.carry == 0 && state.condFlags.sign == 0 &&
                state.condFlags.zero == 0 && state.condFlags.parity == 0 && state.condFlags.auxCarry == 0;
        if (!passed)
            BOOST_ERROR("ADC C failure");
        state.a = 0x42;
        state.condFlags.carry = 1;
        dis.runCycle(state);
        passed = state.a == 0x80 && state.condFlags.carry == 0 && state.condFlags.sign == 1 &&
                state.condFlags.zero == 0 && state.condFlags.parity == 0 && state.condFlags.auxCarry == 1;
        if (!passed)
            BOOST_ERROR("next ADC C failure");
    }
    { // OP SUB A
        state.clearAll();
        state.a = 0x3E;
        memory[0] = 0x97;
        dis.runCycle(state);
        bool passed = state.a == 0 && state.condFlags.carry == 0 && state.condFlags.auxCarry == 1 &&
                state.condFlags.parity == 1 && state.condFlags.zero == 1 && state.condFlags.sign == 0;
        if (!passed)
            BOOST_ERROR("SUB A failure");
    }
    { // OP SBB L
        state.clearAll();
        state.l = 0x2;
        state.a = 0x4;
        state.condFlags.carry = 1;
        memory[0] = 0x9D;
        dis.runCycle(state);
        bool passed = state.a == 0x01 && state.condFlags.carry == 0 && state.condFlags.zero == 0 && state.condFlags.carry == 0 &&
                state.condFlags.auxCarry == 1 && state.condFlags.parity == 0 && state.condFlags.sign == 0;
        if (!passed)
            BOOST_ERROR("SBB L failure");
    }
    { // OP CMP E
        state.clearAll();
        state.condFlags.zero = state.condFlags.carry = 1;
        state.a = 0x0a;
        state.e = 0x05;
        memory[0] = 0xBB;
        dis.runCycle(state);
        bool passed = state.a == 0x0a && state.e == 0x05 &&
                state.condFlags.zero == 0 && state.condFlags.carry == 0;
        if (!passed)
            BOOST_ERROR("CMP E failure");

    }

}

BOOST_AUTO_TEST_CASE( stack_tests ) {
    State8080 state;
    std::array<uint8_t, State8080::RAM>& memory = state.memory;
    Disassembler8080 dis;
    {// OP_POP B 0xc1
        state.clearAll();
        memory[0] = 0xC1;
        state.stackPointer = 0x1508;
        memory[0x1508] = 0x33;
        memory[0x1509] = 0x0B;
        dis.runCycle(state);
        bool passed = state.stackPointer == 0x1508 + 2 && state.b == 0x0B && state.c == 0x33;
        if (!passed)
            BOOST_ERROR("0xC1 OP POP B failure");

    }
    {// OP_PUSH B 0xC5
        state.clearAll();
        memory[0] = 0xC5;
        state.b = 0x8F;
        state.c = 0x9D;
        state.stackPointer = 0x3A2C;
        dis.runCycle(state);
        // note that there is an error in the example in the manual, SP is suppost to be 0x3A2A instead of 0x3A3A.
        bool passed = state.stackPointer == 0x3A2A && state.memory[state.stackPointer] == 0x9D
                && state.memory[0x3A2B] == 0x8F;
        if (!passed)
            BOOST_ERROR("0xC5 OP PUSH B failure");
    }
    { // Push and pop PSW, 0xF5 and 0xF1
        state.clearAll();
        state.a = 0x1F;
        state.stackPointer = 0x502A;
        state.condFlags.carry = state.condFlags.zero = state.condFlags.parity = 1;
        ConditionFlags flagCopy = state.condFlags;
        memory[0] = 0xF5;
        dis.runCycle(state);
        bool passed = memory[0x5029] == 0x1F && 0x47 == memory[0x5028] && state.stackPointer == 0x5028;
        if (!passed)
            BOOST_FAIL("PUSH PSW failure, cannot continue POP test.");
        // pop now
        memory[1] = 0xF1;
        dis.runCycle(state);
        passed = state.condFlags.makeBitSet() == flagCopy.makeBitSet() && state.stackPointer == 0x502A;
        if (!passed)
            BOOST_ERROR("POP PSW failure");
    }
}

BOOST_AUTO_TEST_CASE( branch_tests) {
    State8080 state;
    std::array<uint8_t, State8080::RAM>& memory = state.memory;
    Disassembler8080 dis;
    { // JMP & JNZ 0xC2
        state.clearAll();
        uint16_t jump = 0xFA2B;
        state.condFlags.zero = 1;
        memory[0] = 0xC2;
        memory[1] = 0x2B;
        memory[2] = 0xFA;
        memory[3] = 0xFF;
        memory[jump] = 0xAA;
        dis.runCycle(state);
        if (memory[state.programCounter] != 0xFF)
            BOOST_FAIL("JNZ and JMP opcode failure, cannot continue next test");
        state.condFlags.zero = 0;
        ++state.programCounter;
        memory[4] = 0xC2;
        memory[5] = 0x2B;
        memory[6] = 0xFA;
        dis.runCycle(state);
        if (memory[state.programCounter] != 0xAA)
            BOOST_ERROR("JNZ failure");

    }
    { // RET & CALLADR 0xC9 & 0xCD
        state.clearAll();
        state.stackPointer = 0xE000;
        memory.fill(0x03); // fill will opcode that will indicate an unimplemented instruction
        state.programCounter = 0x10;
        memory[0x10] = 0xCD; // CALLADR
        memory[0x11] = 0xA3;
        memory[0x12] = 0x0B;
        memory[0x13] = 0xEE;
        memory[0x0BA3] = 0xFF; // 0x0BA3 will be the subroutine we will call to.
        memory[0x0BA3 + 1] = 0xC9; // RET
        dis.runCycle(state);
        bool passed = memory[state.programCounter] == 0xFF // check if it jumped
                && memory[state.stackPointer] == 0x13 - 1 // check if lowest bit is set to return addr.
                && memory[state.stackPointer + 1] == 0; // check if highest, program counter is incremnted on return to become 0x13
        if (!passed)
            BOOST_FAIL("RET and CALLADR failure, cannot continue next test");
        ++state.programCounter; // skip the 0xFF
        dis.runCycle(state);
        if (memory[state.programCounter] != 0xEE)
            BOOST_ERROR("RET failure");

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
    { // 0xEB XCHG
       state.clearAll();
       memory[0] = 0xEB;
       state.h = 0;
       state.l = 0xFF;
       state.d = 0x33;
       state.e = 0x55;
       dis.runCycle(state);
       bool passed = state.d == 0 && state.e == 0xFF && state.h == 0x33 && state.l == 0x55;
       if (!passed)
            BOOST_ERROR("0xEB XCHG failure");
    }
    { // XTHL 0xE3
        state.clearAll();
        memory[0] = 0xE3;
        state.stackPointer = 0x10AD;
        state.h = 0x0B;
        state.l = 0x3C;
        memory[0x10AD] = 0xF0;
        memory[0x10AE] = 0x0D;
        dis.runCycle(state);
        bool passed = memory[0x10AD] == 0x3C && memory[0x10AE] == 0x0B && state.h == 0x0D && state.l == 0xF0;
        if (!passed)
            BOOST_ERROR("0xE3 XTHL failure");
    }
    { // SPHL 0xF9
        state.clearAll();
        state.h = 0x50;
        state.l = 0x6C;
        memory[0] = 0xF9;
        dis.runCycle(state);
        if (state.stackPointer != 0x506C)
            BOOST_ERROR("0xF9 SPHL failure");
    }
    {// OP CPI D8 0xfe
        state.clearAll();
        state.condFlags.zero = state.condFlags.carry = state.condFlags.sign = state.condFlags.parity = 1;
        memory[0] = 0xFE;
        state.a = 0x4A;
        memory[1] = 0x40;
        dis.runCycle(state);
        bool passed = state.a == 0x4A && state.condFlags.zero == 0 && state.condFlags.carry == 0;
        if (!passed)
            BOOST_ERROR("0xFE CPI D8 failure");

    }
}

#endif
