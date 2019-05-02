#ifndef STATE_HPP
#define STATE_HPP

#include <cstdint>
#include <array>

#include "ConditionFlags.hpp"

struct State8080 {

    State8080();
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
    static constexpr uint16_t RAM = 0xFFFF;
    std::array<uint8_t, RAM> memory;
    ConditionFlags condFlags;

    void clearMemory();
    void clearRegisters();
    void clearSpecial();
    void clearFlags();
    void clearAll();
};

extern State8080 stateFromFile(const std::string&);
extern State8080 stateFromFile(const std::string&, const uint16_t&);

#endif // STATE_H
