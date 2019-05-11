#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <bitset>

#include "State8080.hpp"
#include "Disassembler8080.hpp"
#include "ConditionFlags.hpp"

int main() {
    ConditionFlags flags;
    std::bitset<8> bits;
    bits[0] = 1;
    bits[2] = 1;
    bits[4] = 1;
    bits[6] = 1;
    bits[7] = 1;
    flags.fromBitSet(bits);
    ConditionFlags copy = flags;
    flags.fromPSW(flags.makePSW());

    return 0;
}

