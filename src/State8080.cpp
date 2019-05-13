#include <fstream>
#include <iostream>
#include <numeric>

#include "State8080.hpp"

State8080::State8080() {
    clearAll();
}

void State8080::clearMemory() {
    std::fill(memory.begin(), memory.end(), 0);
}
void State8080::clearRegisters() {
    a = 0;
    b = 0;
    c = 0;
    d = 0;
    e = 0;
    h = 0;
    l = 0;
}

void State8080::clearSpecial() {
    stackPointer = 0;
    programCounter = 0;
}

void State8080::clearFlags() {
    condFlags.clear();
}

void State8080::clearAll() {
    clearMemory();
    clearRegisters();
    clearSpecial();
    clearFlags();
}



State8080 stateFromFile(const std::string& fname, const uint16_t& offset = 0){

    State8080 state;

    std::ifstream ifs(fname, std::ios_base::binary | std::ios_base::in | std::ios_base::ate);
    if (!ifs.good()) {
        std::cerr << "Invaders file not found" << std::endl;
        throw std::runtime_error("Invaders file not found, given path:" + fname);
    }

    // Make sure there is enough room for the file
    auto size = ifs.tellg();
    ifs.seekg(std::ios_base::beg);
    if (State8080::RAM - offset < size)
        throw std::runtime_error("start of selection of memory is too large or the file itself is too large");

    // load memory to position memoryStart
    uint8_t byte;
    for (size_t i = offset; ifs.read(reinterpret_cast<char*>(&byte), sizeof(uint8_t)) ;i++) {
        state.memory[i] = byte;
    }

    // make sure atleast some data was read, doesn't check the integrity of the file itself.
    if (std::accumulate(state.memory.cbegin() + offset, state.memory.cend(), 0) != 0){
        std::cout << "Successful write into memory" << std::endl;
    }
    else {
        std::cout << "Failed write into memory" << std::endl;
        throw std::runtime_error("Unable to load file contents into memory state");
    }
    return state;
}
