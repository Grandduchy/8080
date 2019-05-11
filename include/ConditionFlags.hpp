#ifndef CONDTIONFLAGS_HPP
#define CONDTIONFLAGS_HPP

#include <cstdint>
#include <bitset>

struct ConditionFlags {

    uint8_t sign : 1;
    uint8_t zero : 1;
    uint8_t parity : 1;
    uint8_t carry : 1;
    uint8_t auxCarry : 1;
    uint8_t padding : 3;

    std::bitset<8> makeBitSet() const noexcept;
    void fromBitSet(const std::bitset<8>&) noexcept;
    uint8_t makePSW() const noexcept;
    void fromPSW(const uint8_t&) noexcept;
    void clear();
};

#endif // CONDTIONFLAGS_HPP
