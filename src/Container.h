#ifndef LAB8_BASE_CONTAINER_H
#define LAB8_BASE_CONTAINER_H

#include <functional>
#include "Object.h"

//interface
template <typename T> class Container : public Object {

public:

    /*
     * Query item by index as function or operator
     * */
    virtual T get(int index) const = 0;
    virtual T operator[](int index) const = 0;

    /*
     * Persist items
     * */
    virtual void add(T item) = 0;
    //inserts before given position
    virtual void add(T item, int index) = 0;

    /*
     * Empty all items
     * */
    virtual void clear() = 0;

    /*
     * Tell if item persisted inside
     * */
    virtual bool contains(T item) const = 0;

    /*
     * Returns index of element
     * */
    virtual int indexOf(T item) const = 0;

    /*
     * Returns last index of persisted items
     * */
    virtual int lastIndex() const = 0;

    /*
     * Amount of items persisted inside
     * */
    virtual int size() const = 0;

    /*
     * Tell if no items persisted inside
     * */
    virtual bool isEmpty() const = 0;

    /*
     * Remove element from persisted items
     * */
    virtual void remove(int index) = 0;

};


#endif //LAB8_BASE_CONTAINER_H
