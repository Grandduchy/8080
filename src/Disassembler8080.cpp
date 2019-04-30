
#include <string>
#include <iostream>
#include <numeric>
#include <bitset>

#include "Disassembler8080.hpp"

#define DEBUG

#define EXECOPCODE(obj, ptr) ((obj).*(ptr))() // executes a pointer to a function member with the object
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
}


void Disassembler8080::runCycle() {
    uint8_t opcode = state.memory[state.programCounter];
    opcodePtr opcodeFunc = opcodeTable[opcode];
    EXECOPCODE(*this, opcodeFunc);
    if (opcodeFunc == &Disassembler8080::unimplemented){
#ifdef DEBUG
        std::cout << opcode;
#endif
        std::cout << std::endl;
    }

    state.programCounter++;
}



void Disassembler8080::setState(const State8080& state) {
    this->state = state;
}
State8080 Disassembler8080::getState() const {
    return state;
}


void Disassembler8080::todo() {

}

void Disassembler8080::unimplemented() {
    std::cout << "Unimplemented Instruction ";
}

void Disassembler8080::OP_NOP() {
    // no work needed.
}

// Set pair group B to the next two bytes
void Disassembler8080::OP_LXIB_D16() {
    state.c = state.memory[state.programCounter + 1];
    state.b = state.memory[state.programCounter + 2];
    state.programCounter += 2;
}

void Disassembler8080::OP_DCRB() {
    --state.b;
    SETZERO(state.b);
    SETSIGN(state.b);
    SETPARITY(state.b);
    // Note that we did not modify AUX flag.
}

// Set register B to the next byte
void Disassembler8080::OP_MVIB_D8() {
    state.b = state.memory[state.programCounter + 1];
    ++state.programCounter;
}

// Register pair B & C is added to H & L register pair
void Disassembler8080::OP_DADB() {
    // combine B & C and H & L
    uint16_t BC = static_cast<uint16_t>( (static_cast<uint16_t>(state.b) << 8) | state.c);
    uint16_t HL = static_cast<uint16_t>( (static_cast<uint16_t>(state.h) << 8) | state.l);
    uint32_t sum = BC + HL;
    SETCARRY(sum, 0xFFFF);
    sum &= 0xFFFF;
    // set H & L to the new sum.
    state.l = sum & 0x00FF;
    state.h = (sum & 0xFF00) >> 8;
}

void Disassembler8080::OP_DCRC() {
    --state.c;
    SETZERO(state.c);
    SETSIGN(state.c);
    SETPARITY(state.c);
}


