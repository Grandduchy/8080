#ifndef STATE_HPP
#define STATE_HPP

#include <cstdint>
#include <array>
#include <algorithm>

#include "ConditionFlags.hpp"

#define RAM 0x10000

struct State8080 {
    State8080(); // defined in hpp because code is short.
    uint16_t stackPointer;
    uint16_t programCounter;

    // General Purpose Registers
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint8_t a; //accumulator register
    std::array<uint8_t, RAM> memory;
    ConditionFlags condFlags;

};


State8080::State8080() {
    std::fill(memory.begin(), memory.end(), 0);
}


#endif // STATE_H
