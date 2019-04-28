
#include <string>
#include <iostream>
#include <fstream>
#include <numeric>

#include "Disassembler8080.hpp"

State8080 Disassembler8080::stateFromFile(const std::string& fname, const uint16_t& memoryStart = 0) const {

    State8080 state;

    std::ifstream ifs(fname, std::ios_base::binary | std::ios_base::in | std::ios_base::ate);
    if (!ifs.good()) {
        std::cerr << "Invaders file not found" << std::endl;
        throw std::runtime_error("Invaders file not found, given path:" + fname);
    }

    // Make sure there is enough room for the file
    auto size = ifs.tellg();
    ifs.seekg(std::ios_base::beg);
    if (RAM - memoryStart < size)
        throw std::runtime_error("start of selection of memory is too large or the file itself is too large");

    // load memory to position memoryStart
    uint8_t byte;
    for (size_t i = memoryStart; ifs.read(reinterpret_cast<char*>(&byte), sizeof(uint8_t)) ;i++) {
        state.memory[i] = byte;
    }

    // make sure atleast some data was read, doesn't check the integrity of the file itself.
    if (std::accumulate(state.memory.cbegin() + memoryStart, state.memory.cend(), 0) != 0){
        std::cout << "Successful write into memory" << std::endl;
    }
    else {
        std::cout << "Failed write into memory" << std::endl;
        throw std::runtime_error("Unable to load file contents into memory state");
    }
    return state;
}

void Disassembler8080::runCycle(State8080 & state) {
    uint8_t opcode = state.memory[state.programCounter];

}

