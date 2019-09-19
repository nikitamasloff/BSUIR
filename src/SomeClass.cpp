#include <iostream>
#include "SomeClass.h"

SomeClass::SomeClass() = default;

SomeClass::~SomeClass() {
    std::cout << "destructor of 'SomeClass'\n";
}

void SomeClass::print() {
    std::cout << "this is 'SomeClass'\n" << this;
}

void SomeClass::someFun() {
    std::cout << "some function of 'SomeClass'\n";
}

std::ostream &operator<<(std::ostream &os, const SomeClass &aClass) {
    os << aClass << " someProtectedField: " << aClass.someProtectedField
       << " somePrivateField: " << aClass.somePrivateField << '\n';
    return os;
}
