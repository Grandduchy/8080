#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#include "State8080.hpp"

int main(int argc, char* argv[]) {
    std::string fname("invaders");
    if (argc == 2) // name of path is given
        fname = std::string(argv[1]);

    State8080 state;

    std::ifstream ifs(fname, std::ios_base::binary | std::ios_base::in);
    if (!ifs.good()) {
        std::cerr << "Invaders file not found" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    // load memory to position 0
    uint8_t byte;
    for (size_t i = 0; ifs.read(reinterpret_cast<char*>(&byte), sizeof(uint8_t)) ;i++) {
        state.memory[i] = byte;
    }
    auto nonZero = std::find_if_not(state.memory.begin(), state.memory.end(), [](uint8_t& byte){
        return byte == 0;
    });

    if (nonZero != state.memory.end()){
        std::cout << "Successful write into memory" << std::endl;
    }
    else {
        std::cout << "Failed write into memory" << std::endl;
    }
    return 0;
}
