#ifndef LAB8_SOMECLASS_H
#define LAB8_SOMECLASS_H


#include <ostream>
#include "Object.h"

class SomeClass : public Object {

public:

    SomeClass();
    virtual ~SomeClass() override;
    void print() override;
    //unique function
    void someFun();

    friend std::ostream &operator<<(std::ostream &os, const SomeClass &aClass);

protected:
    int someProtectedField = 3;

private:
    int somePrivateField = 4;

};


#endif //LAB8_SOMECLASS_H
