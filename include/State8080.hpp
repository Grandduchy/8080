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
    uint32_t cycleCount = 0;
    // Variables for shift registering, both LHS and RHS create a uint16_t
    uint8_t shiftLHS = 0;
    uint8_t shiftRHS = 0;
    int shiftOffset = 0; // offset to shift from [0, 15] but starts from reverse direction

    // Ports for I/O instructions
    uint8_t port1 = 0;
    uint8_t port2 = 0;
    uint8_t port3 = 0;
    uint8_t port5 = 0;

    ConditionFlags condFlags;
    bool allowInterrupt = false;
    bool halted = false;
    void clearMemory();
    void clearRegisters();
    void clearSpecial();
    void clearFlags();
    void clearAll();
};

extern State8080 stateFromFile(const std::string&, const uint16_t& offset);

#endif // STATE_H
