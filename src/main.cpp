
#include "tester.h"

#if !defined(OP_TEST) && !defined(sideTest)

#include <iostream>
#include <chrono>
#include <vector>

int main() {

    auto lastInterrupt = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
                );
    auto q = 1000000000;
    std::vector<int>a(q);
    for (int i = 0; i != q; i++)
        a.push_back(i);
    auto n = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
                );
    std::cout << (n - lastInterrupt).count() << std::endl;

    return 0;
}


#endif
