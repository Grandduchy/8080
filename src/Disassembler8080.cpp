
#include <string>
#include <iostream>
#include <numeric>
#include <bitset>

#include "Disassembler8080.hpp"

#define EXECOPCODE(obj, ptr, state) ((obj).*(ptr))(state) // executes a pointer to a function member with the object

Disassembler8080::Disassembler8080() {
    for (auto& opcodePtr : opcodeTable) {
        opcodePtr = &Disassembler8080::unimplemented;
    }

    /// Carry Bit Instructions
    opcodeTable[0x37] = &Disassembler8080::OP_STC;
    opcodeTable[0x3F] = &Disassembler8080::OP_CMC;


    /// Immediate Instructions
    opcodeTable[0x06] = &Disassembler8080::OP_MVIB_D8;
    opcodeTable[0x0e] = &Disassembler8080::OP_MVIC_D8;
    opcodeTable[0x16] = &Disassembler8080::OP_MVID_D8;
    opcodeTable[0x1E] = &Disassembler8080::OP_MVIE_D8;
    opcodeTable[0x26] = &Disassembler8080::OP_MVIH_D8;
    opcodeTable[0x2E] = &Disassembler8080::OP_MVIL_D8;
    opcodeTable[0x36] = &Disassembler8080::OP_MVIM_D8;
    opcodeTable[0x3E] = &Disassembler8080::OP_MVIA_D8;

    opcodeTable[0x21] = &Disassembler8080::OP_LXIH_D16;
    opcodeTable[0x01] = &Disassembler8080::OP_LXIB_D16;
    opcodeTable[0x11] = &Disassembler8080::OP_LXID_D16;
    opcodeTable[0x31] = &Disassembler8080::OP_LXISP_D16;

    opcodeTable[0xC6] = &Disassembler8080::OP_ADI_D8;
    opcodeTable[0xCE] = &Disassembler8080::OP_ACI_D8;
    opcodeTable[0xD6] = &Disassembler8080::OP_SUI_D8;
    opcodeTable[0xDE] = &Disassembler8080::OP_SBI_D8;
    opcodeTable[0xE6] = &Disassembler8080::OP_ANI_D8;
    opcodeTable[0xEE] = &Disassembler8080::OP_XRI_D8;
    opcodeTable[0xF6] = &Disassembler8080::OP_ORI_D8;
    opcodeTable[0xFE] = &Disassembler8080::OP_CPI_D8;



    /// Direct Addressing Instructions
    opcodeTable[0x32] = &Disassembler8080::OP_STA_ADR;
    opcodeTable[0x3A] = &Disassembler8080::OP_LDA_ADR;
    opcodeTable[0x22] = &Disassembler8080::OP_SHLD;
    opcodeTable[0x2A] = &Disassembler8080::OP_LHLD;

    /// Jump instructions
    for (std::size_t i = 0xC2; i != 0xFA + 8; i+= 8) {
        opcodeTable[i] = &Disassembler8080::OP_JUMP;
    }
    opcodeTable[0xC3] = &Disassembler8080::OP_JUMP;
    opcodeTable[0xCB] = &Disassembler8080::OP_JUMP;


    /// Call subroutine instructions
    opcodeTable[0xCD] = &Disassembler8080::OP_CALL;

    /// Return from subroutine instructions
    opcodeTable[0xC8] = &Disassembler8080::OP_RZ;
    opcodeTable[0xC9] = &Disassembler8080::OP_RET;


    /// RST instructions

    /// Interrupt instructions
    opcodeTable[0xF3] = &Disassembler8080::OP_DI;
    opcodeTable[0xFB] = &Disassembler8080::OP_EI;


    /// I/O instructions
    opcodeTable[0xD3] = &Disassembler8080::OP_OUTD8;
    opcodeTable[0xDB] = &Disassembler8080::OP_IND8;


    /// Single Register Instructions
    opcodeTable[0x2F] = &Disassembler8080::OP_CMA;
    opcodeTable[0x27] = &Disassembler8080::OP_DAA;
        // DCR
    opcodeTable[0x05] = &Disassembler8080::OP_DCRB;
    opcodeTable[0x0d] = &Disassembler8080::OP_DCRC;
    opcodeTable[0x15] = &Disassembler8080::OP_DCRD;
    opcodeTable[0x1D] = &Disassembler8080::OP_DCRE;
    opcodeTable[0x25] = &Disassembler8080::OP_DCRH;
    opcodeTable[0x2D] = &Disassembler8080::OP_DCRL;
    opcodeTable[0x35] = &Disassembler8080::OP_DCRM;
    opcodeTable[0x3D] = &Disassembler8080::OP_DCRA;
        // INR
    opcodeTable[0x04] = &Disassembler8080::OP_INRB;
    opcodeTable[0x0C] = &Disassembler8080::OP_INRC;
    opcodeTable[0x14] = &Disassembler8080::OP_INRD;
    opcodeTable[0x1C] = &Disassembler8080::OP_INRE;
    opcodeTable[0x24] = &Disassembler8080::OP_INRH;
    opcodeTable[0x2C] = &Disassembler8080::OP_INRL;
    opcodeTable[0x34] = &Disassembler8080::OP_INRM;
    opcodeTable[0x3C] = &Disassembler8080::OP_INRA;


    /// NOP instruction
    opcodeTable[0x0] = &Disassembler8080::OP_NOP;
    opcodeTable[0x10] = &Disassembler8080::OP_NOP;
    opcodeTable[0x20] = &Disassembler8080::OP_NOP;
    opcodeTable[0x30] = &Disassembler8080::OP_NOP;
    opcodeTable[0x08] = &Disassembler8080::OP_NOP;
    opcodeTable[0x18] = &Disassembler8080::OP_NOP;
    opcodeTable[0x28] = &Disassembler8080::OP_NOP;
    opcodeTable[0x38] = &Disassembler8080::OP_NOP;


    /// Data transfer Instructions
    opcodeTable[0x1a] = &Disassembler8080::OP_LDAXD;
    // Load all 56 move instructions
    for (std::size_t i = 0x40; i != 0x80; i++) {
        if (i == 0x76) continue; // HALT, not a data transfer instruction
        opcodeTable[i] = &Disassembler8080::OP_MOV;
    }
    opcodeTable[0x02] = &Disassembler8080::OP_STAXB;
    opcodeTable[0x12] = &Disassembler8080::OP_STAXD;
    opcodeTable[0x0A] = &Disassembler8080::OP_LDAXB;

    /// Register or memory to accumulator instructions
    for (std::size_t i = 0x80; i != 0xC0; i++) {
        opcodeTable[i] = &Disassembler8080::OP_REG_ACC;
    }

    /// Roatate accumulator instructions
    opcodeTable[0x0f] = &Disassembler8080::OP_RRC;
    opcodeTable[0x07] = &Disassembler8080::OP_RLC;
    opcodeTable[0x17] = &Disassembler8080::OP_RAL;
    opcodeTable[0x1F] = &Disassembler8080::OP_RAR;


    /// Register Pair Instructions
        // DAD
    opcodeTable[0x09] = &Disassembler8080::OP_DADB;
    opcodeTable[0x19] = &Disassembler8080::OP_DADD;
    opcodeTable[0x29] = &Disassembler8080::OP_DADH;
    opcodeTable[0x39] = &Disassembler8080::OP_DADSP;
        // INX
    opcodeTable[0x03] = &Disassembler8080::OP_INXB;
    opcodeTable[0x13] = &Disassembler8080::OP_INXD;
    opcodeTable[0x23] = &Disassembler8080::OP_INXH;
    opcodeTable[0x33] = &Disassembler8080::OP_INXSP;
        // DCX
    opcodeTable[0x0B] = &Disassembler8080::OP_DCXB;
    opcodeTable[0x1B] = &Disassembler8080::OP_DCXD;
    opcodeTable[0x2B] = &Disassembler8080::OP_DCXH;
    opcodeTable[0x3B] = &Disassembler8080::OP_DCXSP;
        // PUSH/POP
    opcodeTable[0xC1] = &Disassembler8080::OP_POPB;
    opcodeTable[0xC5] = &Disassembler8080::OP_PUSHB;
    opcodeTable[0xD1] = &Disassembler8080::OP_POPD;
    opcodeTable[0xD5] = &Disassembler8080::OP_PUSHD;
    opcodeTable[0xE1] = &Disassembler8080::OP_POPH;
    opcodeTable[0xE5] = &Disassembler8080::OP_PUSHH;
    opcodeTable[0xF1] = &Disassembler8080::OP_POPPSW;
    opcodeTable[0xF5] = &Disassembler8080::OP_PUSHPSW;

    opcodeTable[0xE3] = &Disassembler8080::OP_XTHL;
    opcodeTable[0xEB] = &Disassembler8080::OP_XCHG;
    opcodeTable[0xF9] = &Disassembler8080::OP_SPHL;


    // Halt Instruction

}


void Disassembler8080::runCycle(State8080& state) {
    uint8_t opcode = state.memory[state.programCounter];
    opcodePtr opcodeFunc = opcodeTable[opcode];
    EXECOPCODE(*this, opcodeFunc, state);
    wasUnimplemented = opcodeFunc == &Disassembler8080::unimplemented;
    wasTodo = opcodeFunc == &Disassembler8080::todo;
    if (opcodeFunc == &Disassembler8080::OP_IND8 || opcodeFunc == &Disassembler8080::OP_OUTD8)
        state.programCounter++;
    state.programCounter++;
}

void Disassembler8080::generateInterrupt(State8080& state) {
    // push PC onto the stack
    uint16_t returnAddress = state.programCounter - 1; // -5 works for ~42476
    state.memory[state.stackPointer - 1] = (returnAddress >> 8) & 0xFF; // store high bit
    state.memory[state.stackPointer - 2] = returnAddress & 0xFF; // store low bit
    state.stackPointer -= 2;
    // while there are 8 different numbers other than 2 for RST,
    // space invaders only uses number 2.
    state.programCounter = 8 * 2;
}

inline void Disassembler8080::setZero(State8080& state, const uint16_t& expr) const noexcept {
    state.condFlags.zero = ((expr & 0xFF) == 0) ? 1 : 0;
}
inline void Disassembler8080::setSign(State8080& state, const uint16_t& expr) const noexcept {
    state.condFlags.sign = ((expr & 0x80) != 0) ? 1 : 0;
}
inline void Disassembler8080::setCarry(State8080& state, const uint32_t& expr, const uint16_t& maxVal) const noexcept {
    state.condFlags.carry = expr > maxVal;
}
inline void Disassembler8080::setParity(State8080& state, const uint8_t& expr) const noexcept {
 // if possible use GNU's optimised function for popcount
#ifdef __GNUC__
    state.condFlags.parity = ((__builtin_popcount(expr) & 0xFF) & 0x1) != 1;
#else
    state.condFlags.parity = (std::bitset<8>(expr).count() & 0x1) != 1;
#endif
}
inline void Disassembler8080::setAux8(State8080& state, const uint8_t& expr) const noexcept {
    state.condFlags.auxCarry = (expr & 0xF) != 0xF;
}
inline void Disassembler8080::setAux16(State8080& state, const uint16_t& expr) const noexcept {
    state.condFlags.auxCarry = (expr & 0xFF) != 0xFF;
}

/// ------------- Everything below this is to do with opcodes --------------------


// General opcode functions that are done multiple times in each different opcode function

// set register pair to next two bytes
inline void Disassembler8080::LXI_D16(State8080& state, uint8_t& firstRegPair, uint8_t& secondRegPair) {
    secondRegPair = state.memory[state.programCounter + 1];
    firstRegPair = state.memory[state.programCounter + 2];
    state.programCounter += 2;
}

// Decrement a register
inline void Disassembler8080::DCR(State8080& state, uint8_t& reg) {
    --reg;
    setZero(state, reg);
    setSign(state, reg);
    setParity(state, reg);
    setAux8(state, reg);
}

// Increment a register
inline void Disassembler8080::INR(State8080& state, uint8_t& reg) {
    ++reg;
    setZero(state, reg);
    setSign(state, reg);
    setParity(state, reg);
    setAux8(state, reg);
}


// Register pair reg1 & reg2 is added to H & L register pair
inline void Disassembler8080::DAD(State8080& state, uint8_t& regPair1, uint8_t& regPair2) {
    // combine the pairs
    uint16_t pair = static_cast<uint16_t>( (static_cast<uint16_t>(regPair1) << 8) | regPair2);
    uint16_t HL = static_cast<uint16_t>( (static_cast<uint16_t>(state.h) << 8) | state.l);
    uint32_t sum = pair + HL;
    setCarry(state, sum, 0xFFFF);
    sum &= 0xFFFF;
    // set H & L to the new sum.
    state.l = sum & 0x00FF;
    state.h = (sum & 0xFF00) >> 8;
}

// set register to next byte
inline void Disassembler8080::MVI_D8(State8080& state, uint8_t& reg) {
    reg = state.memory[state.programCounter + 1];
    ++state.programCounter;
}

// A register pair is incrmeented by one and stored into the pair
inline void Disassembler8080::INX(uint8_t& regPair1,uint8_t& regPair2) {
    // combine the registers into a pair
    uint16_t pair = static_cast<uint16_t>( (static_cast<uint16_t>(regPair1) << 8) | regPair2);
    ++pair;
    // store into the registers
    regPair1 = (pair & 0xFF00) >> 8;
    regPair2 = pair & 0x00FF;
}

// A register pair is decremented by one and stored into the pair, same as INX
inline void Disassembler8080::DCX(uint8_t& regPair1, uint8_t regPair2) {
    uint16_t pair = static_cast<uint16_t>( (static_cast<uint16_t>(regPair1) << 8) | regPair2);
    --pair;
    regPair1 = (pair & 0xFF00) >> 8;
    regPair2 = pair & 0x00FF;
}

inline void Disassembler8080::MOV(uint8_t& dst, uint8_t& src) {
    dst = src;
}
inline void Disassembler8080::MOV_SRC(State8080& state, uint8_t& src) {
    state.memory[static_cast<uint16_t>((static_cast<uint16_t>(state.h) << 8) | state.l  )] = src;
}
inline void Disassembler8080::MOV_DST(State8080& state, uint8_t& dst) {
    dst = state.memory[static_cast<uint16_t>((static_cast<uint16_t>(state.h) << 8) | state.l  )];
}

// Pop memory in the stack to the register pair regPair1 & regPair2
inline void Disassembler8080::POP(State8080& state, uint8_t& regPair1, uint8_t& regPair2) {
    regPair2 = state.memory[state.stackPointer];
    regPair1 = state.memory[state.stackPointer + 1];
    state.stackPointer += 2;
}

// Push the register pair into the stack
inline void Disassembler8080::PUSH(State8080& state, uint8_t& regPair1, uint8_t& regPair2) {
    state.memory[state.stackPointer - 1] = regPair1;
    state.memory[state.stackPointer - 2] = regPair2;
    state.stackPointer -= 2;
}

// Add a register to the accumulator with the carry flag
inline void Disassembler8080::ADC(State8080& state, const uint8_t& reg) {
    uint16_t sum = state.a + reg + state.condFlags.carry;
    state.a = sum & 0xFF;
    setSign(state, sum);
    setParity(state, sum & 0xFF);
    setZero(state, sum);
    setAux8(state, sum & 0xFF);
    setCarry(state,sum, 0xFF);
}

// Add a register to the accumulator
inline void Disassembler8080::ADD(State8080& state, const uint8_t& reg) {
    uint16_t sum = state.a + reg;
    state.a = sum & 0xFF;
    setSign(state, sum);
    setParity(state, sum & 0xFF);
    setZero(state, sum);
    setAux8(state, sum & 0xFF);
    setCarry(state,sum, 0xFF);
}

// Subtract register from accumulator
inline void Disassembler8080::SUB(State8080& state, const uint8_t& reg) {
    uint8_t regComp = ~reg + 1; // produce two's complement using reg
    uint16_t sum = state.a + regComp;
    state.a = sum & 0xFF;
    // setting of carry is inversed here because the numbers differ in sign
    state.condFlags.carry = sum > 0xFF ? 0 : 1;
    setParity(state, state.a);
    setZero(state, state.a);
    setSign(state, state.a);
    setAux8(state, state.a);
}

// Subtract register from accumulator but with borrow
inline void Disassembler8080::SBB(State8080& state, const uint8_t& reg) {
    // Same thing as sub, but + carry bit is added to the reg first
    uint8_t newReg = reg + state.condFlags.carry;
    SUB(state, newReg);
}

// Binary XOR the register with the accumulator
inline void Disassembler8080::XRA(State8080& state, const uint8_t& reg) {
    state.a ^= reg;
    setParity(state, state.a);
    setZero(state, state.a);
    setSign(state, state.a);
    setAux8(state, state.a);
    state.condFlags.carry = 0;
}

// Binary AND the register with the accumulator
inline void Disassembler8080::ANA(State8080& state, const uint8_t& reg) {
    state.a &= reg;
    setParity(state, state.a);
    setZero(state, state.a);
    setSign(state, state.a);
    state.condFlags.carry = 0;
}

// Binary OR the register with the accumulator
inline void Disassembler8080::ORA(State8080& state, const uint8_t& reg) {
    state.a |= reg;
    setParity(state, state.a);
    setZero(state, state.a);
    setSign(state, state.a);
    state.condFlags.carry = 0;
}

// Subtract the register from the accumulator BUT do not modify the registers,
// only modification of the flags
inline void Disassembler8080::CMP(State8080& state, const uint8_t& reg) {
    uint8_t regComp = ~reg + 1; // two's complement
    uint16_t sum = state.a + regComp;
    // carry is inversed
    // sum is compared not the registers
    state.condFlags.carry = sum > 0xFF ? 0 : 1;
    setParity(state, sum & 0xFF);
    setZero(state, sum);
    setSign(state, sum);
    setAux8(state, sum & 0xFF);

}



/********************/
// Opcode functions //
/********************/




void Disassembler8080::todo(State8080& state) {
    std::cout << std::hex;
    std::cout << "Instruction " << static_cast<int>(state.memory[state.programCounter]) << " marked as todo \n";
    std::cout << std::dec;
}

void Disassembler8080::unimplemented(State8080& state) {
    std::cout << std::hex;
    std::cout << "Instruction " << static_cast<int>(state.memory[state.programCounter]) << " marked as unimplemented \n";
    std::cout << std::dec;
}


///// CARRY BIT INSTRUCTIONS


// set carry to 1
void Disassembler8080::OP_STC(State8080& state) {
    state.condFlags.carry = 1;
}

// complement the carry flag, like a toggle.
void Disassembler8080::OP_CMC(State8080& state) {
    state.condFlags.carry = !state.condFlags.carry;
}


///// IMMEDIATE INSTRUCTIONS


// Set pair group B to the next two bytes
void Disassembler8080::OP_LXIB_D16(State8080& state) {
    LXI_D16(state, state.b, state.c);
}

// Set register B to the next byte
void Disassembler8080::OP_MVIB_D8(State8080& state) {
    MVI_D8(state, state.b);
}

void Disassembler8080::OP_MVIC_D8(State8080& state) {
    MVI_D8(state, state.c);
}

void Disassembler8080::OP_MVID_D8(State8080& state) {
    MVI_D8(state, state.d);
}

void Disassembler8080::OP_MVIE_D8(State8080& state) {
    MVI_D8(state, state.e);
}

void Disassembler8080::OP_MVIH_D8(State8080& state) {
    MVI_D8(state, state.h);
}

void Disassembler8080::OP_MVIL_D8(State8080& state) {
    MVI_D8(state, state.l);
}

void Disassembler8080::OP_MVIM_D8(State8080& state) {
    // function won't work for this, store the byte into some location denoted by H & l pair
    uint16_t address = static_cast<uint16_t>( (static_cast<uint16_t>(state.h) << 8) | state.l);
    state.memory[address] = state.memory[state.programCounter + 1];
    ++state.programCounter;
}

void Disassembler8080::OP_MVIA_D8(State8080& state) {
    MVI_D8(state, state.a);
}

void Disassembler8080::OP_LXID_D16(State8080& state) {
    LXI_D16(state, state.d, state.e);
}

void Disassembler8080::OP_LXIH_D16(State8080& state) { // 0x21
    LXI_D16(state, state.h, state.l);
}

void Disassembler8080::OP_LXISP_D16(State8080& state) {
    // the function won't work for this.
    uint8_t byte1 = state.memory[state.programCounter + 1];
    uint8_t byte2 = state.memory[state.programCounter + 2];
    uint16_t address = static_cast<uint16_t>( (static_cast<uint16_t>(byte2) << 8) | byte1);
    state.stackPointer = address;
    state.programCounter += 2;
}

// Adds the next byte to the accumulator
void Disassembler8080::OP_ADI_D8(State8080& state) {
    uint16_t sum = state.a + state.memory[state.programCounter + 1];
    setZero(state, sum);
    setParity(state, sum & 0xFF);
    setSign(state, sum);
    setCarry(state, sum, 0xFF);
    setAux8(state, sum & 0xFF);
    state.a = 0xFF & sum;
    ++state.programCounter;
}

// Adds the next byte to the accumulator, but with carry.
void Disassembler8080::OP_ACI_D8(State8080& state) {
    uint16_t sum = state.a + state.memory[state.programCounter + 1] + state.condFlags.carry;
    setZero(state, sum);
    setParity(state, sum & 0xFF);
    setSign(state, sum);
    setCarry(state, sum, 0xFF);
    setAux8(state, sum & 0xFF);
    state.a = 0xFF & sum;
    ++state.programCounter;
}

// Subtract the next byte from the accumulator
void Disassembler8080::OP_SUI_D8(State8080& state) {
    // this is needed to know if there was a carry
    uint8_t subByte = ~state.memory[state.programCounter + 1] + 1; // subtracted byte using two's complement
    uint16_t sum = state.a + subByte;
    setZero(state, sum);
    setParity(state, sum & 0xFF);
    setSign(state, sum);
    // carry is inversed here, set to 1 if no carry
    state.condFlags.carry = sum > 0xFF ? 0 : 1;
    setAux8(state, sum & 0xFF);
    state.a = sum & 0xFF;
    ++state.programCounter;
}

// Subtract the next byte added with the carry bit from the accumulator
void Disassembler8080::OP_SBI_D8(State8080& state) {
    uint8_t subByte = ~(state.memory[state.programCounter + 1] + state.condFlags.carry) + 1; // subtracted byte using two's complement
    uint16_t sum = state.a + subByte;
    setZero(state, sum);
    setParity(state, sum & 0xFF);
    setSign(state, sum);
    // carry is inversed here, set to 1 if no carry
    state.condFlags.carry = sum > 0xFF ? 0 : 1;
    setAux8(state, sum & 0xFF);
    state.a = sum & 0xFF;
    ++state.programCounter;
}

// perform binary AND with next byte with the accumulator
void Disassembler8080::OP_ANI_D8(State8080& state) {
    state.a &= state.memory[state.programCounter + 1];
    state.condFlags.carry = 0;
    setParity(state, state.a);
    setZero(state, state.a);
    setSign(state, state.a);
    ++state.programCounter;
}

// Perform binary XOR with next byte with the accumulator
void Disassembler8080::OP_XRI_D8(State8080& state) {
    state.a ^= state.memory[state.programCounter + 1];
    state.condFlags.carry = 0;
    setParity(state, state.a);
    setZero(state, state.a);
    setSign(state, state.a);
    ++state.programCounter;
}

// Perform binary OR with next byte with the accumulator
void Disassembler8080::OP_ORI_D8(State8080& state) {
    state.a  |= state.memory[state.programCounter + 1];
    state.condFlags.carry = 0;
    setParity(state, state.a);
    setZero(state, state.a);
    setSign(state, state.a);
    ++state.programCounter;
}


// compare next byte with accumulator by subtracting, note that nothing is set.
void Disassembler8080::OP_CPI_D8(State8080& state) {
    uint8_t byte = state.memory[state.programCounter + 1];
    uint16_t sum = state.a - byte;
    state.condFlags.zero = byte == state.a;
    setParity(state, sum & 0xFF);
    setSign(state, sum);
    state.condFlags.carry = sum > byte;
    setAux8(state, sum & 0xFF);
    ++state.programCounter;
}


/////// DIRECT ADDRESSING INSTRUCTIONS


// store address of the accumulator to memory location at next two bytes.
void Disassembler8080::OP_STA_ADR(State8080& state) {
    uint8_t lowByte = state.memory[state.programCounter + 1];
    uint8_t highByte = state.memory[state.programCounter + 2];
    uint16_t address = static_cast<uint16_t>( (static_cast<uint16_t>(highByte) << 8) | lowByte);
    state.memory[address] = state.a;
    state.programCounter += 2;
}
// Set accumulator to the memory stored by the address of the next two bytes
void Disassembler8080::OP_LDA_ADR(State8080& state) {
    uint8_t lowByte = state.memory[state.programCounter + 1];
    uint8_t highByte = state.memory[state.programCounter + 2];
    uint16_t address = static_cast<uint16_t>( (static_cast<uint16_t>(highByte) << 8) | lowByte);
    state.a = state.memory[address];
    state.programCounter += 2;
}

// L is stored at the address of the next two bytes
// H is stored at the address + 1
void Disassembler8080::OP_SHLD(State8080& state){
    uint8_t lowByte = state.memory[state.programCounter + 1];
    uint8_t highByte = state.memory[state.programCounter + 2];
    uint16_t address = static_cast<uint16_t>( (static_cast<uint16_t>(highByte) << 8) | lowByte);
    state.memory[address] = state.l;
    state.memory[address + 1] = state.h;
    state.programCounter += 2;
}

// Opposite of SHLD, L is loaded from the address of the next two bytes
// H is loaded from the address + 1
void Disassembler8080::OP_LHLD(State8080& state){
    uint8_t lowByte = state.memory[state.programCounter + 1];
    uint8_t highByte = state.memory[state.programCounter + 2];
    uint16_t address = static_cast<uint16_t>( (static_cast<uint16_t>(highByte) << 8) | lowByte);
    state.l = state.memory[address];
    state.h = state.memory[address + 1];
    state.programCounter += 2;
}


//////// JUMP INSTRUCTIONS

void Disassembler8080::OP_JUMP(State8080& state) {
    uint8_t opcode = state.memory[state.programCounter];
    switch(opcode) {
        case 0xC3: // JMP
        case 0xCB: JUMP(state, true); break;
        case 0xDA: JUMP(state, state.condFlags.carry == 1); break; // JC
        case 0xD2: JUMP(state, state.condFlags.carry == 0); break; // JNC
        case 0xCA: JUMP(state, state.condFlags.zero == 1); break; // JZ
        case 0xC2: JUMP(state, state.condFlags.zero == 0); break; // JNZ
        case 0xFA: JUMP(state, state.condFlags.sign == 1); break; // JM
        case 0xF2: JUMP(state, state.condFlags.sign == 0); break; // JP
        case 0xEA: JUMP(state, state.condFlags.parity == 1); break; //JPE
        case 0xE2: JUMP(state, state.condFlags.parity == 0); break; // JPO
        default:
            throw std::runtime_error("opcode doesn't have a jump command opcode (dec) : " + std::to_string(static_cast<int>(opcode)) );
    }
}


inline void Disassembler8080::JUMP(State8080& state, bool canJump) const noexcept {
    if (canJump) {
        uint8_t lowAdd = state.memory[state.programCounter + 1];
        uint8_t hiAdd = state.memory[state.programCounter + 2];
        uint16_t addr = static_cast<uint16_t>((static_cast<uint16_t>(hiAdd) << 8) | lowAdd);
        state.programCounter = addr;
        --state.programCounter; // program counter will automatically be incremented, so do the inverse
    }
    else { // jump is not taken
        state.programCounter += 2;
    }
}


//////// CALL SUBROUTINE INSTRUCTIONS


// call a subroutine
void Disassembler8080::OP_CALL(State8080& state) { // 0xcd
    uint16_t newAddress = static_cast<uint16_t>((state.memory[state.programCounter + 2] << 8) | state.memory[state.programCounter + 1]);
    // store the old address to the stack, it's pushed onto the stack
    uint16_t returnAddress = state.programCounter + 2; // skip to the next instruction after this one
    state.memory[state.stackPointer - 1] = (returnAddress >> 8) & 0xFF; // store high bit
    state.memory[state.stackPointer - 2] = returnAddress & 0xFF; // store low bit
    state.stackPointer -= 2;
    state.programCounter = newAddress;
    --state.programCounter; // inverse the increment of the program counter
 }


//////// RETURN FROM SUBROUTINE INSTRUCTIONS


// return from a subroutine if zero is set
void Disassembler8080::OP_RZ(State8080& state) {
    if (state.condFlags.zero == 1)
        OP_RET(state);
}

// return from a subroutine
void Disassembler8080::OP_RET(State8080& state) {
    // pop the old address from the stack
    state.programCounter = static_cast<uint16_t>(state.memory[state.stackPointer] | (state.memory[state.stackPointer + 1] << 8) );
    state.stackPointer += 2;
    // increment of PC is ok here, returnAddress is purposely set to 2 instead of 3 to note of this.
}


//////// RST INSTRUCTIONS



////// INTERRUPT INSTRUCTIONS


void Disassembler8080::OP_EI(State8080& state) {
    state.allowInterrupt = true;
}

void Disassembler8080::OP_DI(State8080 & state) {
    state.allowInterrupt = false;
}


////// I/O INSTRUCTIONS


void Disassembler8080::OP_IND8(State8080 &){}
void Disassembler8080::OP_OUTD8(State8080 &){}



////// SINGLE REGISTER INSTRUCTIONS


// Every bit of the accumulator is inverted
void Disassembler8080::OP_CMA(State8080& state) {
    state.a = ~state.a;
}

void Disassembler8080::OP_DAA(State8080& state) {
    // if the 4 least sig bits are > 9
    if ((state.a & 0xF) > 9 || state.condFlags.auxCarry == 1) {
        state.condFlags.auxCarry = (((state.a & 0xF) + 6) & 0xF0) != 0; // set if carry out of least 4
        state.a += 6;
    }
    // if the 4 great sig bits are > 9
    if ((state.a & 0xF0) > 0x90 || state.condFlags.carry == 1) {
        uint16_t sum = static_cast<uint16_t>(state.a) + 0x60;
        state.a = sum & 0xFF;
        setParity(state, state.a);
        setZero(state, state.a);
        setSign(state, state.a);
        setCarry(state, sum, 0xFF);

    }
}

void Disassembler8080::OP_INRB(State8080& state) { // 0x04
    INR(state, state.b);
}
void Disassembler8080::OP_INRC(State8080& state) {// 0x0C
    INR(state, state.c);
}
void Disassembler8080::OP_INRD(State8080& state) { // 0x14
    INR(state, state.d);
}
void Disassembler8080::OP_INRE(State8080& state) { // 0x1C
    INR(state, state.e);
}
void Disassembler8080::OP_INRH(State8080& state) { // 0x24
    INR(state, state.h);
}
void Disassembler8080::OP_INRL(State8080& state) { // 0x2C
    INR(state, state.l);
}
void Disassembler8080::OP_INRM(State8080& state) { // 0x34
    uint16_t address = static_cast<uint16_t>((static_cast<uint16_t>(state.h) << 8) | state.l);
    uint8_t& byte = state.memory[address];
    INR(state, byte);
}
void Disassembler8080::OP_INRA(State8080& state) { // 0x3C
    INR(state, state.a);
}


void Disassembler8080::OP_DCRB(State8080& state) {
    DCR(state, state.b);
}
void Disassembler8080::OP_DCRC(State8080& state) {
    DCR(state, state.c);
}
void Disassembler8080::OP_DCRD(State8080& state) {
    DCR(state, state.d);
}
void Disassembler8080::OP_DCRE(State8080& state) {
    DCR(state, state.e);
}
void Disassembler8080::OP_DCRH(State8080& state) {
    DCR(state, state.h);
}
void Disassembler8080::OP_DCRL(State8080& state) {
    DCR(state, state.l);
}

void Disassembler8080::OP_DCRM(State8080& state) {
    uint16_t HL = static_cast<uint16_t>((static_cast<uint16_t>(state.h) << 8) | state.l);
    uint8_t& byte = state.memory[HL];
    DCR(state, byte);
}
void Disassembler8080::OP_DCRA(State8080& state) {
    DCR(state, state.a);
}


////// NOP INSTRUCTION


void Disassembler8080::OP_NOP(State8080&) {
    // no work needed
}


////// DATA TRANSFER INSTRUCTIONS


// Contents of the accumulator is replaced by the location in memory denoted by the register pair
void Disassembler8080::OP_LDAXD(State8080& state) {
    uint16_t location = static_cast<uint16_t>( (static_cast<uint16_t>(state.d) << 8) | state.e);
    state.a = state.memory[location];
}
void Disassembler8080::OP_LDAXB(State8080& state) {
    uint16_t location = static_cast<uint16_t>( (static_cast<uint16_t>(state.b) << 8) | state.c);
    state.a = state.memory[location];
}

// use a jump table for each move instruction
// MOV_REG is only for a mov operation between two registers
// each register for a mov between two have the same format in which register to move to
// but is displaced by a number, this can be found on the opcode table
// MOV_DST is different, the source is a memory location
// MOV_SRC is similarly different, the destination is a memory location (not a register)
// Function saves the creation of ~ 45 functions
#define MOV_REG(number, reg) \
    case 0x40 + (number): MOV(reg, state.b); break; \
    case 0x41 + (number): MOV(reg, state.c); break; \
    case 0x42 + (number): MOV(reg, state.d); break; \
    case 0x43 + (number): MOV(reg, state.e); break; \
    case 0x44 + (number): MOV(reg, state.h); break; \
    case 0x45 + (number): MOV(reg, state.l); break; \
    case 0x47 + (number): MOV(reg, state.a); break;

void Disassembler8080::OP_MOV(State8080& state) {
    uint8_t opcode = state.memory[state.programCounter];
    switch(opcode) {
        MOV_REG(0, state.b)
        MOV_REG(8, state.c)
        MOV_REG(16, state.d)
        MOV_REG(24, state.e)
        MOV_REG(32, state.h)
        MOV_REG(40, state.l)
        MOV_REG(56, state.a)
        case 0x46: MOV_DST(state, state.b); break;
        case 0x56: MOV_DST(state, state.d); break;
        case 0x66: MOV_DST(state, state.h); break;
        case 0x4E: MOV_DST(state, state.c); break;
        case 0x5E: MOV_DST(state, state.e); break;
        case 0x6E: MOV_DST(state, state.l); break;
        case 0x7E: MOV_DST(state, state.a); break;

        case 0x70: MOV_SRC(state, state.b); break;
        case 0x71: MOV_SRC(state, state.c); break;
        case 0x72: MOV_SRC(state, state.d); break;
        case 0x73: MOV_SRC(state, state.e); break;
        case 0x74: MOV_SRC(state, state.h); break;
        case 0x75: MOV_SRC(state, state.l); break;
        case 0x77: MOV_SRC(state, state.a); break;
        default:
            throw std::runtime_error("No move operation for opcode (dec) :" + std::to_string(static_cast<int>(opcode)) );
    }
}
#undef MOV_REG

// Contents of accumulator is placed into memory location denoted by register pair
void Disassembler8080::OP_STAXB(State8080& state) {
    uint16_t address = static_cast<uint16_t>((static_cast<uint16_t>(state.b) << 8) | state.c);
    state.memory[address] = state.a;

}
void Disassembler8080::OP_STAXD(State8080& state) {
    uint16_t address = static_cast<uint16_t>((static_cast<uint16_t>(state.d) << 8) | state.e);
    state.memory[address] = state.a;
}



////////// REGISTER OR MEMORY TO ACCUMULATOR INSTRUCTIONS

// numSrc is the location of where the function starts on the opcode table
// func is the function to apply to each opcode
// created via the observation that 56 opcodes are in the same format but just
// different functions, opcodes 0x80 - 0xBF all have a function that operates on 8 different registers but all do the same thing
// effectively saves the creation of 56 functions
// case numSrc + 6 line refers to the register to add to is located by the memory denoted by registers H & L
#define ACCSWITCH(func, numSrc) \
    case numSrc + 0: func(state, state.b); break; \
    case numSrc + 1: func(state, state.c); break; \
    case numSrc + 2: func(state, state.d); break; \
    case numSrc + 3: func(state, state.e); break; \
    case numSrc + 4: func(state, state.h); break; \
    case numSrc + 5: func(state, state.l); break; \
    case numSrc + 6: func(state, state.memory[static_cast<uint16_t>((static_cast<uint16_t>(state.h) << 8) | state.l)]); break; \
    case numSrc + 7: func(state, state.a); break; \


void Disassembler8080::OP_REG_ACC(State8080& state) {
    uint8_t opcode = state.memory[state.programCounter];
    switch(opcode) {
        ACCSWITCH(ADD, 0x80)
        ACCSWITCH(ADC, 0x88)
        ACCSWITCH(SUB, 0x90)
        ACCSWITCH(SBB, 0x98)
        ACCSWITCH(ANA, 0xA0)
        ACCSWITCH(XRA, 0xA8)
        ACCSWITCH(ORA, 0xB0)
        ACCSWITCH(CMP, 0xB8)
        default:
            throw std::runtime_error("No register to accumulator instruction for opcode (dec): " + std::to_string(static_cast<int>(opcode)) );
    }
}

#undef ACCSWITCH

/////// ROTATE ACCUMULATOR INSTRUCTIONS

// Rotate the accumulator to the right
void Disassembler8080::OP_RRC(State8080& state) {
    uint8_t lowestOrderBit = state.a & 0x1;
    state.a >>= 1;
    // reference indicates to move the bit to the highest order
    state.a |= (lowestOrderBit << 7);
    state.condFlags.carry = lowestOrderBit;
}

// Rotate the accumulator to the left
void Disassembler8080::OP_RLC(State8080& state) {
    uint8_t highestOrderBit = (state.a & 0x80) >> 7;
    state.a <<= 1;
    state.a |= highestOrderBit; // move the bit to the lowest order
    state.condFlags.carry = highestOrderBit;
}

// Rotate the accumulator to the left with a carry
// Rotate the accumulator, then swap the highest order bit with the carry bit
// no wrapping is done in these two functions.
void Disassembler8080::OP_RAL(State8080& state) {
    uint8_t highestOrderBit = (state.a & 0x80) >> 7;
    state.a <<= 1;
    state.a |= (state.condFlags.carry << 7); // put carry into highest order bit
    state.condFlags.carry = highestOrderBit; // put hob into carry;
}

// Rotate the accumulator to the right with carry
void Disassembler8080::OP_RAR(State8080& state) {
    uint8_t lowestOrderBit = state.a & 0x1;
    state.a >>= 1;
    state.a |= (state.condFlags.carry << 7); // put carry into highest order bit
    state.condFlags.carry = lowestOrderBit;
}



/////// REGISTER PAIR INSTRUCTIONS


void Disassembler8080::OP_DADB(State8080& state) {
    DAD(state, state.b, state.c);
}

void Disassembler8080::OP_DADD(State8080& state) {
    DAD(state, state.d, state.e);
}

void Disassembler8080::OP_DADH(State8080& state) {
    DAD(state, state.h, state.l);
}

void Disassembler8080::OP_DADSP(State8080& state) {
    uint8_t upperSP = (state.stackPointer & 0xFF00) >> 8;
    uint8_t lowerSP = state.stackPointer & 0xFF;
    DAD(state, upperSP, lowerSP);
}


void Disassembler8080::OP_POPB(State8080& state) {
    POP(state, state.b, state.c);
}

void Disassembler8080::OP_PUSHB(State8080& state) {
    PUSH(state, state.b, state.c);
}

void Disassembler8080::OP_POPD(State8080& state) {
    POP(state, state.d, state.e);
}

void Disassembler8080::OP_PUSHD(State8080& state) {
    PUSH(state, state.d, state.e);
}

void Disassembler8080::OP_POPH(State8080& state) {
    POP(state, state.h, state.l);
}

void Disassembler8080::OP_PUSHH(State8080& state) {
    PUSH(state, state.h, state.l);
}

// pop/push data onto stack w/ PSW and accumulator
void Disassembler8080::OP_POPPSW(State8080& state) {
    uint8_t PSW = 0;
    POP(state, state.a, PSW);
    state.condFlags.fromPSW(PSW);
}

// pop/push data onto stack w/ PSW and accumulator
void Disassembler8080::OP_PUSHPSW(State8080& state) {
    uint8_t PSW = state.condFlags.makePSW();
    PUSH(state, state.a, PSW);
}

void Disassembler8080::OP_INXB(State8080& state) {
    INX(state.b, state.c);
}

void Disassembler8080::OP_INXD(State8080& state) {
    INX(state.d, state.e);
}

void Disassembler8080::OP_INXH(State8080& state) {
    INX(state.h, state.l);
}

void Disassembler8080::OP_INXSP(State8080& state) {
    uint8_t upperSP = (state.stackPointer & 0xFF00) >> 8;
    uint8_t lowerSP = state.stackPointer & 0xFF;
    INX(upperSP, lowerSP);
}

void Disassembler8080::OP_DCXB(State8080& state) {
    DCX(state.b, state.c);
}

void Disassembler8080::OP_DCXD(State8080& state) {
    DCX(state.d, state.e);
}

void Disassembler8080::OP_DCXH(State8080& state) {
    DCX(state.h, state.l);
}

void Disassembler8080::OP_DCXSP(State8080& state) {
    uint8_t upperSP = (state.stackPointer & 0xFF00) >> 8;
    uint8_t lowerSP = state.stackPointer & 0xFF;
    DCX(upperSP, lowerSP);
}

// swap pairs h&l with d&e
void Disassembler8080::OP_XCHG(State8080& state) {
    std::swap(state.h, state.d);
    std::swap(state.l, state.e);
}

// Exchange the stack
// L and the byte by SP are swapped
// H and the byte by SP + 1 are swapped
// SP does not change
void Disassembler8080::OP_XTHL(State8080& state) {
    uint8_t& byteSP = state.memory[state.stackPointer];
    uint8_t& byteSP1 = state.memory[state.stackPointer + 1];
    std::swap(byteSP, state.l);
    std::swap(byteSP1, state.h);
}

// SP now points to the address by the H & L registers
void Disassembler8080::OP_SPHL(State8080& state) {
    uint16_t HL = static_cast<uint16_t>((static_cast<uint16_t>(state.h) << 8) | state.l);
    state.stackPointer = HL;
}
