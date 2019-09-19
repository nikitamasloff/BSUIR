#include <iostream>
#include "Object.h"

Object::~Object() {
    std::cout << "destructor of 'Object'\n";
}

void Object::print() {
    std::cout << "this is 'Object'\n";
}
