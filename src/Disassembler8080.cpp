
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
    state.c = state.memory[state.programCounter + 1];
    state.b = state.memory[state.programCounter + 2];
    state.programCounter += 2;
}

void Disassembler8080::OP_DCRB(State8080& state) {
    --state.b;
    SETZERO(state.b);
    SETSIGN(state.b);
    SETPARITY(state.b);
    // Note that we did not modify AUX flag.
}

// Set register B to the next byte
void Disassembler8080::OP_MVIB_D8(State8080& state) {
    state.b = state.memory[state.programCounter + 1];
    ++state.programCounter;
}

// Register pair B & C is added to H & L register pair
void Disassembler8080::OP_DADB(State8080& state) {
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

void Disassembler8080::OP_DCRC(State8080& state) {
    --state.c;
    SETZERO(state.c);
    SETSIGN(state.c);
    SETPARITY(state.c);
}

void Disassembler8080::OP_MVIC_D8(State8080& state) {
    state.c = state.memory[state.programCounter + 1];
    ++state.programCounter;
}

void Disassembler8080::OP_RRC(State8080& state) {
    uint8_t lowestOrderBit = state.a & 0x1;
    state.a >>= 1;
    // reference indicates to move the bit to the highest order
    state.a |= (lowestOrderBit << 7);
    state.condFlags.carry = (lowestOrderBit == 1);
}

void Disassembler8080::OP_LXID_D16(State8080& state) {
    state.e = state.memory[state.programCounter + 1];
    state.d = state.memory[state.programCounter + 2];
    state.programCounter += 2;
}

// Register pair D & E are incremented by one and stored into the pair.
void Disassembler8080::OP_INXD(State8080& state) {
    // combine D & E
    uint16_t DE = static_cast<uint16_t>( (static_cast<uint16_t>(state.d) << 8) | state.e);
    ++DE;
    // store into the registers
    state.d = (DE & 0xFF00) >> 8;
    state.e = DE & 0x00FF;
}

void Disassembler8080::OP_DADD(State8080& state) {
    uint16_t DE = static_cast<uint16_t>( (static_cast<uint16_t>(state.d) << 8) | state.e);
    uint16_t HL = static_cast<uint16_t>( (static_cast<uint16_t>(state.h) << 8) | state.l);
    uint32_t sum = DE + HL;
    SETCARRY(sum, 0xFFFF);
    sum &= 0xFFFF;
    // set H & L to the new sum.
    state.l = sum & 0x00FF;
    state.h = (sum & 0xFF00) >> 8;
}

void Disassembler8080::OP_LDAXD(State8080& state) {
    uint16_t location = static_cast<uint16_t>( (static_cast<uint16_t>(state.d) << 8) | state.e);
    state.a = state.memory[location];

}

// ?
void Disassembler8080::OP_LXIH_D16(State8080& state) { // 0x21
    state.l = state.memory[state.programCounter + 1];
    state.h = state.memory[state.programCounter + 2];
    state.programCounter += 2;

}

