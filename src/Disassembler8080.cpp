
#include <string>
#include <iostream>
#include <fstream>
#include <numeric>

#include "Disassembler8080.hpp"

Disassembler8080::Disassembler8080() {
    for (auto& opcodePtr : opcodeTable) {
        opcodePtr = &Disassembler8080::unimplemented;
    }
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




