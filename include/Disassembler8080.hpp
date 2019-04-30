#ifndef DISASSEMBLER8080_HPP
#define DISASSEMBLER8080_HPP

#include <array>

#include "State8080.hpp"

#define EXECOPCODE(obj, ptr) ((obj).*(ptr))() // executes a pointer to a function member with the object

class Disassembler8080 {
public:
    using opcodePtr = void (Disassembler8080::*)(); // the type of pointer to opcode member functions

    Disassembler8080();
    void runCycle();
    std::array<opcodePtr, 256> opcodeTable;

    void setState(const State8080&);
    State8080 getState() const;

private:
    State8080 state;

    void unimplemented(); // an opcode that is unimplemented and will never be done
    void todo(); // an opcode that is currently unimplemented but will eventually be done

    void OP_NOP();
};

#endif // DISASSEMBLER8080_HPP
