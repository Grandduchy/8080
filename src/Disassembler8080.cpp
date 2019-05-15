
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
    #define SETPARITY(expr) (state.condFlags.parity = (__builtin_popcount((expr) & 0xFF) & 0x1) != 1)
#else
    #define SETPARITY(expr) (state.condFlags.parity = (( std::bitset<sizeof(decltype(expr)) * 8>((expr) & 0xFF).count()) & 0x1 ) != 1 )
#endif

#define SETAUX_8(expr) (state.condFlags.auxCarry = ((expr) & 0xF) == 0xF)
#define SETAUX_16(expr) (state.condFlags.auxCarry = ((expr) & 0xFF) == 0xFF)


#define UNUSED(param) (void)(param)

Disassembler8080::Disassembler8080() {
    for (auto& opcodePtr : opcodeTable) {
        opcodePtr = &Disassembler8080::unimplemented;
    }

    /// Carry Bit Instructions
    opcodeTable[0x37] = &Disassembler8080::OP_STC;
    opcodeTable[0x3F] = &Disassembler8080::OP_CMC;


    /// Immediate Instructions
    opcodeTable[0x01] = &Disassembler8080::OP_LXIB_D16;
    opcodeTable[0x06] = &Disassembler8080::OP_MVIB_D8;
    opcodeTable[0x0e] = &Disassembler8080::OP_MVIC_D8;
    opcodeTable[0x11] = &Disassembler8080::OP_LXID_D16;
    opcodeTable[0x21] = &Disassembler8080::OP_LXIH_D16;
    opcodeTable[0x26] = &Disassembler8080::OP_MVIH_D8;
    opcodeTable[0x31] = &Disassembler8080::OP_LXISP_D16;
    opcodeTable[0x36] = &Disassembler8080::OP_MVIM_D8;
    opcodeTable[0x3E] = &Disassembler8080::OP_MVIA_D8;
    opcodeTable[0xC6] = &Disassembler8080::OP_ADID8;
    opcodeTable[0xE6] = &Disassembler8080::OP_ANID8;
    opcodeTable[0xFE] = &Disassembler8080::OP_CPI_D8;



    /// Direct Addressing Instructions
    opcodeTable[0x32] = &Disassembler8080::OP_STA_ADR;
    opcodeTable[0x3A] = &Disassembler8080::OP_LDA_ADR;


    /// Jump instructions
    opcodeTable[0xC2] = &Disassembler8080::OP_JNZ;
    opcodeTable[0xC3] = &Disassembler8080::OP_JMP;
    opcodeTable[0xCA] = &Disassembler8080::OP_JZ;
    opcodeTable[0xDA] = &Disassembler8080::OP_JC;

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
    opcodeTable[0x80] = &Disassembler8080::OP_ADDB;
    opcodeTable[0x81] = &Disassembler8080::OP_ADDC;
    opcodeTable[0x82] = &Disassembler8080::OP_ADDD;
    opcodeTable[0x83] = &Disassembler8080::OP_ADDE;
    opcodeTable[0x84] = &Disassembler8080::OP_ADDH;
    opcodeTable[0x85] = &Disassembler8080::OP_ADDL;
    opcodeTable[0x86] = &Disassembler8080::OP_ADDM;
    opcodeTable[0x87] = &Disassembler8080::OP_ADDA;
    opcodeTable[0x88] = &Disassembler8080::OP_ADCB;
    opcodeTable[0x89] = &Disassembler8080::OP_ADCC;
    opcodeTable[0x8A] = &Disassembler8080::OP_ADCD;
    opcodeTable[0x8B] = &Disassembler8080::OP_ADCE;
    opcodeTable[0x8C] = &Disassembler8080::OP_ADCH;
    opcodeTable[0x8D] = &Disassembler8080::OP_ADCL;
    opcodeTable[0x8E] = &Disassembler8080::OP_ADCM;
    opcodeTable[0x8F] = &Disassembler8080::OP_ADCA;


    opcodeTable[0xA7] = &Disassembler8080::OP_ANAA;
    opcodeTable[0xAF] = &Disassembler8080::OP_XRAA;


    /// Roatate accumulator instructions
    opcodeTable[0x0f] = &Disassembler8080::OP_RRC;


    /// Register Pair Instructions
    opcodeTable[0x09] = &Disassembler8080::OP_DADB;
    opcodeTable[0x13] = &Disassembler8080::OP_INXD;
    opcodeTable[0x19] = &Disassembler8080::OP_DADD;
    opcodeTable[0x23] = &Disassembler8080::OP_INXH;
    opcodeTable[0x29] = &Disassembler8080::OP_DADH;
    opcodeTable[0xC1] = &Disassembler8080::OP_POPB;
    opcodeTable[0xC5] = &Disassembler8080::OP_PUSHB;
    opcodeTable[0xD1] = &Disassembler8080::OP_POPD;
    opcodeTable[0xD5] = &Disassembler8080::OP_PUSHD;
    opcodeTable[0xE1] = &Disassembler8080::OP_POPH;
    opcodeTable[0xE5] = &Disassembler8080::OP_PUSHH;
    opcodeTable[0xEB] = &Disassembler8080::OP_XCHG;
    opcodeTable[0xF1] = &Disassembler8080::OP_POPPSW;
    opcodeTable[0xF5] = &Disassembler8080::OP_PUSHPSW;


    // Halt Instruction

}


void Disassembler8080::runCycle(State8080& state) {
    uint8_t opcode = state.memory[state.programCounter];
    opcodePtr opcodeFunc = opcodeTable[opcode];
    EXECOPCODE(*this, opcodeFunc, state);
    if (opcodeFunc == &Disassembler8080::unimplemented)
        wasUnimplemented = true;
    if (opcodeFunc == &Disassembler8080::todo)
        wasTodo = true;
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
    SETZERO(reg);
    SETSIGN(reg);
    SETPARITY(reg);
    SETAUX_8(reg);
}

// Increment a register
inline void Disassembler8080::INR(State8080& state, uint8_t& reg) {
    ++reg;
    SETZERO(reg);
    SETSIGN(reg);
    SETPARITY(reg);
    SETAUX_8(reg);
}


// Register pair reg1 & reg2 is added to H & L register pair
inline void Disassembler8080::DAD(State8080& state, uint8_t& regPair1, uint8_t& regPair2) {
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

// Add a register to the accumulator, op_add can use this function iff carry = 0
inline void Disassembler8080::ADC(State8080& state, uint8_t& reg, const uint8_t& carry) {
    uint16_t sum = state.a + reg + carry;
    SETSIGN(sum);
    SETPARITY(sum);
    SETZERO(sum);
    SETAUX_8(sum & 0xFF);
    SETCARRY(sum, 0xFF);
}


/********************/
// Opcode functions //
/********************/




void Disassembler8080::todo(State8080& state) {
    std::cout << std::hex;
    std::cout << "Instruction " << static_cast<int>(state.memory[state.programCounter]) << " marked as todo \n";
    UNUSED(state);
    std::cout << std::dec;
}

void Disassembler8080::unimplemented(State8080& state) {
    std::cout << std::hex;
    std::cout << "Instruction " << static_cast<int>(state.memory[state.programCounter]) << " marked as unimplemented \n";
    UNUSED(state);
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

void Disassembler8080::OP_LXID_D16(State8080& state) {
    LXI_D16(state, state.d, state.e);
}

void Disassembler8080::OP_LXIH_D16(State8080& state) { // 0x21
    LXI_D16(state, state.h, state.l);
}

void Disassembler8080::OP_MVIH_D8(State8080& state) {
    MVI_D8(state, state.h);
}

void Disassembler8080::OP_LXISP_D16(State8080& state) {
    // the function won't work for this.
    uint8_t byte1 = state.memory[state.programCounter + 1];
    uint8_t byte2 = state.memory[state.programCounter + 2];
    uint16_t address = static_cast<uint16_t>( (static_cast<uint16_t>(byte2) << 8) | byte1);
    state.stackPointer = address;
    state.programCounter += 2;
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

// set accumulator to next byte
void Disassembler8080::OP_ADID8(State8080& state) {
    uint16_t sum = state.a + state.memory[state.programCounter + 1];
    SETZERO(sum);
    SETPARITY(sum);
    SETSIGN(sum);
    SETCARRY(sum, 0xFF);
    SETAUX_16(sum);
    state.a = 0xFF & sum;
    ++state.programCounter;
}
// perform binary and with next byte with the accumulator
void Disassembler8080::OP_ANID8(State8080& state) {
    state.a &= state.memory[state.programCounter + 1];
    state.condFlags.carry = 0;
    SETPARITY(state.a);
    SETZERO(state.a);
    SETSIGN(state.a);
    ++state.programCounter;
}

// compare next byte with accumulator by subtracting, note thtat nothing is set.
void Disassembler8080::OP_CPI_D8(State8080& state) {
    uint8_t byte = state.memory[state.programCounter + 1];
    uint16_t sum = state.a - byte;
    state.condFlags.zero = byte == state.a;
    SETPARITY(sum);
    SETSIGN(sum);
    state.condFlags.carry = sum > byte;
    SETAUX_16(sum);
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


//////// JUMP INSTRUCTIONS


// jump to the address if the zero bit is zero
void Disassembler8080::OP_JNZ(State8080& state) {
    if (state.condFlags.zero == 0)
        OP_JMP(state);
    else // jump is not taken
        state.programCounter += 2;
}

// jump to address
void Disassembler8080::OP_JMP(State8080& state) {
    uint8_t lowAdd = state.memory[state.programCounter + 1];
    uint8_t hiAdd = state.memory[state.programCounter + 2];
    uint16_t addr = static_cast<uint16_t>((static_cast<uint16_t>(hiAdd) << 8) | lowAdd);
    state.programCounter = addr;
    --state.programCounter; // program counter will automatically be incremented, so do the inverse
}
void Disassembler8080::OP_JZ(State8080& state) {
    if (state.condFlags.zero == 1)
        OP_JMP(state);
    else
        state.programCounter += 2;
}
void Disassembler8080::OP_JC(State8080& state) {
    if (state.condFlags.carry == 1)
        OP_JMP(state);
    else
        state.programCounter += 2;
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
        SETPARITY(state.a);
        SETZERO(state.a);
        SETSIGN(state.a);
        SETCARRY(sum, 0xFF);

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


void Disassembler8080::OP_NOP(State8080& state) {
    // no work needed.
    UNUSED(state);
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
            throw std::runtime_error("No move operation for opcode " + std::to_string(static_cast<int>(opcode)) );
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

void Disassembler8080::OP_ADDB(State8080& state) {
    ADC(state, state.b, 0);
}
void Disassembler8080::OP_ADDC(State8080& state) {
    ADC(state, state.c, 0);
}
void Disassembler8080::OP_ADDD(State8080& state) {
    ADC(state, state.d, 0);
}
void Disassembler8080::OP_ADDE(State8080& state) {
    ADC(state, state.e, 0);
}
void Disassembler8080::OP_ADDH(State8080& state) {
    ADC(state, state.h, 0);
}
void Disassembler8080::OP_ADDL(State8080& state) {
    ADC(state, state.l, 0);
}
void Disassembler8080::OP_ADDM(State8080& state) {
    uint16_t address = static_cast<uint16_t>((static_cast<uint16_t>(state.h) << 8) | state.l);
    uint8_t& byte = state.memory[address];
    ADC(state, byte, 0);
}
void Disassembler8080::OP_ADDA(State8080& state) {
    ADC(state, state.a, 0);
}

void Disassembler8080::OP_ADCB(State8080& state) {
    ADC(state, state.b, state.condFlags.carry);
}
void Disassembler8080::OP_ADCC(State8080& state) {
    ADC(state, state.c, state.condFlags.carry);
}
void Disassembler8080::OP_ADCD(State8080& state) {
    ADC(state, state.d, state.condFlags.carry);
}
void Disassembler8080::OP_ADCE(State8080& state) {
    ADC(state, state.e, state.condFlags.carry);
}
void Disassembler8080::OP_ADCH(State8080& state) {
    ADC(state, state.h, state.condFlags.carry);
}
void Disassembler8080::OP_ADCL(State8080& state) {
    ADC(state, state.l, state.condFlags.carry);
}
void Disassembler8080::OP_ADCM(State8080& state) {
    uint16_t address = static_cast<uint16_t>((static_cast<uint16_t>(state.h) << 8) | state.l);
    uint8_t& byte = state.memory[address];
    ADC(state, byte, state.condFlags.carry);
}
void Disassembler8080::OP_ADCA(State8080& state) {
    ADC(state, state.a, state.condFlags.carry);
}

// Perform binary and with a and a
void Disassembler8080::OP_ANAA(State8080& state) {
    state.a &= state.a;
    SETPARITY(state.a);
    SETZERO(state.a);
    SETSIGN(state.a);
    state.condFlags.carry = 0;
}

// Perform binary xor with a and a
void Disassembler8080::OP_XRAA(State8080& state) {
    state.a ^= state.a;
    SETPARITY(state.a);
    SETZERO(state.a);
    SETSIGN(state.a);
    state.condFlags.carry = 0;
}


/////// ROTATE ACCUMULATOR INSTRUCTIONS



void Disassembler8080::OP_RRC(State8080& state) {
    uint8_t lowestOrderBit = state.a & 0x1;
    state.a >>= 1;
    // reference indicates to move the bit to the highest order
    state.a |= (lowestOrderBit << 7);
    state.condFlags.carry = (lowestOrderBit == 1);
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

void Disassembler8080::OP_INXD(State8080& state) {
    INX(state.d, state.e);
}

void Disassembler8080::OP_INXH(State8080& state) {
    INX(state.h, state.l);
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

// swap pairs h&l with d&e
void Disassembler8080::OP_XCHG(State8080& state) {
    std::swap(state.h, state.d);
    std::swap(state.l, state.e);
}
