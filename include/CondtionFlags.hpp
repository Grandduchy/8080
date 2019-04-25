#ifndef CONDTIONFLAGS_HPP
#define CONDTIONFLAGS_HPP

#include <cstdint>

struct ConditionFlags {
    uint8_t sign : 1;
    uint8_t zero : 1;
    uint8_t parity : 1;
    uint8_t carry : 1;
    uint8_t auxCarry : 1;
    uint8_t padding : 3;
};


#endif // CONDTIONFLAGS_HPP
