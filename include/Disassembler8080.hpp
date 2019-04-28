#ifndef DISASSEMBLER8080_HPP
#define DISASSEMBLER8080_HPP

#include "State8080.hpp"

class Disassembler8080 {
public:

    State8080 stateFromFile(const std::string&) const;
    State8080 stateFromFile(const std::string&, const uint16_t&) const;

    void runCycle(State8080&);

};

#endif // DISASSEMBLER8080_HPP
