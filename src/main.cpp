#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#include "State8080.hpp"

#define EXEC(obj,ptr) ((obj).*(ptr))()

class Object {
public:
    void hello() {
        std::cout << "Hello World!\n";
    }
    using ptr = void(Object::*)();
};


int main() {

    Object a;

    Object::ptr ptrToHello;
    ptrToHello = &Object::hello;

    //(a.*(ptrToHello))();
    EXEC(a,ptrToHello);
    return 0;
}
