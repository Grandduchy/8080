#include "ConditionFlags.hpp"

// The condition flags can be used as a byte of data to represent the flags
// This function turns the bitfield into a byte where:
// (bit) 0 -> C
// 1 -> 1, is not used
// 2 -> P
// 3 -> 0, is not used
// 4 -> A
// 5 -> 0
// 6 -> Z
// 7 -> S
std::bitset<8> ConditionFlags::makeBitSet() const noexcept{
    std::bitset<8> set(2); // set 1's bit
    set[0] = carry;
    set[2] = parity;
    set[4] = auxCarry;
    set[6] = zero;
    set[7] = sign;
    return set;
}

// does opposite of makeBitSet, fills the flags object
void ConditionFlags::fromBitSet(const std::bitset<8>& set) noexcept{
    carry = set[0];
    parity = set[2];
    auxCarry = set[4];
    zero = set[6];
    sign = set[7];

}

uint8_t ConditionFlags::makePSW() const noexcept {
    uint8_t PSW = 2;
    PSW |= carry;
    PSW |= (parity << 2);
    PSW |= (auxCarry << 4);
    PSW |= (zero << 6);
    PSW |= (sign << 7);
    return PSW;
}
void ConditionFlags::fromPSW(const uint8_t& PSW) noexcept {
    carry = 1 & PSW;
    parity =  (PSW & (1 << 2)) >> 2;
    auxCarry = (PSW & (1 << 4)) >> 4;
    zero = (PSW & (1 << 6)) >> 6;
    sign = (PSW & (1 << 7)) >> 7;
}

void ConditionFlags::clear() {
    carry = 0;
    parity = 0;
    auxCarry = 0;
    zero = 0;
    sign = 0;
}
