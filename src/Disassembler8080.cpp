
#include <string>
#include <iostream>
#include <numeric>
#include <bitset>

#include "Disassembler8080.hpp"

#define DEBUG

#define EXECOPCODE(obj, ptr, state) ((obj).*(ptr))(state) // executes a pointer to a function member with the object
#define SETZERO(expr) (state.condFlags.zero = (((expr) & 0xff) == 0) ? 1 : 0)
#define SETSIGN(expr) (state.condFlags.sign = (((expr) & 0x80) != 0) ? 1 : 0 )
#define SETCARRY(expr, maxVal) (state.condFlags.carry = (expr) > (maxVal))
// count all bits, if sum is even set flag to 1 if odd set to 0
// use gnu's optimised function if possible
#ifdef __GNUC__
    #define SETPARITY(expr) (state.condFlags.parity = !((__builtin_popcount(expr) & 0x1) == 1))
#else
    #define SETPARITY(expr) (state.condFlags.parity = !((( std::bitset<sizeof(decltype(expr)) * 8>(expr).count()) & 0x1 ) == 1 ))
#endif

#define UNUSED(param) (void)(param)

Disassembler8080::Disassembler8080() {
    for (auto& opcodePtr : opcodeTable) {
        opcodePtr = &Disassembler8080::unimplemented;
    }

    opcodeTable[0x0] = &Disassembler8080::OP_NOP;
    opcodeTable[0x01] = &Disassembler8080::OP_LXIB_D16;
    opcodeTable[0x05] = &Disassembler8080::OP_DCRB;
    opcodeTable[0x06] = &Disassembler8080::OP_MVIB_D8;
    opcodeTable[0x09] = &Disassembler8080::OP_DADB;
    opcodeTable[0x0d] = &Disassembler8080::OP_DCRC;
    opcodeTable[0x0e] = &Disassembler8080::OP_MVIC_D8;
    opcodeTable[0x0f] = &Disassembler8080::OP_RRC;
    opcodeTable[0x11] = &Disassembler8080::OP_LXID_D16;
    opcodeTable[0x13] = &Disassembler8080::OP_INXD;
    opcodeTable[0x19] = &Disassembler8080::OP_DADD;
    opcodeTable[0x1a] = &Disassembler8080::OP_LDAXD;
}


void Disassembler8080::runCycle(State8080& state) {
    uint8_t opcode = state.memory[state.programCounter];
    opcodePtr opcodeFunc = opcodeTable[opcode];
    EXECOPCODE(*this, opcodeFunc, state);
    if (opcodeFunc == &Disassembler8080::unimplemented){
#ifdef DEBUG
        std::cout << opcode;
#endif
        std::cout << std::endl;
    }

    state.programCounter++;
}



// General opcode functions that are done multiple times in each different opcode function

// set register pair to next two bytes
void Disassembler8080::LXI_D16(State8080& state, uint8_t& firstRegPair, uint8_t& secondRegPair) {
    secondRegPair = state.memory[state.programCounter + 1];
    firstRegPair = state.memory[state.programCounter + 2];
    state.programCounter += 2;
}

// Decrement a register
void Disassembler8080::DCR(State8080& state, uint8_t& reg) {
    --reg;
    SETZERO(reg);
    SETSIGN(reg);
    SETPARITY(reg);
    // Note that we did not modify AUX flag.
}

// Register pair reg1 & reg2 is added to H & L register pair
void Disassembler8080::DAD(State8080& state, uint8_t& regPair1, uint8_t& regPair2) {
    // combine the pairs
    uint16_t pair = static_cast<uint16_t>( (static_cast<uint16_t>(regPair1) << 8) | regPair2);
    uint16_t HL = static_cast<uint16_t>( (static_cast<uint16_t>(state.h) << 8) | state.l);
    uint32_t sum = pair + HL;
    SETCARRY(sum, 0xFFFF);
    sum &= 0xFFFF;
    // set H & L to the new sum.
    state.l = sum & 0x00FF;
    state.h = (sum & 0xFF00) >> 8;
}

// set register to next byte
void Disassembler8080::MVI_D8(State8080& state, uint8_t& reg) {
    reg = state.memory[state.programCounter + 1];
    ++state.programCounter;
}

// A register pair is incrmeented by one and stored into the pair
void Disassembler8080::INX(uint8_t& regPair1,uint8_t& regPair2) {
    // combine the registers into a pair
    uint16_t pair = static_cast<uint16_t>( (static_cast<uint16_t>(regPair1) << 8) | regPair2);
    ++pair;
    // store into the registers
    regPair1 = (pair & 0xFF00) >> 8;
    regPair2 = pair & 0x00FF;
}

// Opcode functions

void Disassembler8080::todo(State8080& state) {
    UNUSED(state);
}

void Disassembler8080::unimplemented(State8080& state) {
    std::cout << "Unimplemented Instruction ";
    UNUSED(state);
}





void Disassembler8080::OP_NOP(State8080& state) {
    // no work needed.
    UNUSED(state);
}

// Set pair group B to the next two bytes
void Disassembler8080::OP_LXIB_D16(State8080& state) {
    LXI_D16(state, state.b, state.c);
}



void Disassembler8080::OP_DCRB(State8080& state) {
    DCR(state, state.b);

}

// Set register B to the next byte
void Disassembler8080::OP_MVIB_D8(State8080& state) {
    MVI_D8(state, state.b);
}

void Disassembler8080::OP_DADB(State8080& state) {
    DAD(state, state.b, state.c);
}

void Disassembler8080::OP_DCRC(State8080& state) {
    DCR(state, state.c);
}

void Disassembler8080::OP_MVIC_D8(State8080& state) {
    MVI_D8(state, state.c);
}

void Disassembler8080::OP_RRC(State8080& state) {
    uint8_t lowestOrderBit = state.a & 0x1;
    state.a >>= 1;
    // reference indicates to move the bit to the highest order
    state.a |= (lowestOrderBit << 7);
    state.condFlags.carry = (lowestOrderBit == 1);
}

void Disassembler8080::OP_LXID_D16(State8080& state) {
    LXI_D16(state, state.d, state.e);
}

void Disassembler8080::OP_INXD(State8080& state) {
    INX(state.d, state.e);
}

void Disassembler8080::OP_DADD(State8080& state) {
    DAD(state, state.d, state.e);
}

void Disassembler8080::OP_LDAXD(State8080& state) {
    uint16_t location = static_cast<uint16_t>( (static_cast<uint16_t>(state.d) << 8) | state.e);
    state.a = state.memory[location];
}

void Disassembler8080::OP_LXIH_D16(State8080& state) { // 0x21
    LXI_D16(state, state.h, state.l);
}

