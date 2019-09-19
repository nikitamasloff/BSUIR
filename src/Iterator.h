#ifndef LAB8_ITERATOR_H
#define LAB8_ITERATOR_H

//interface
template <typename T> class Iterator {

public:

    /*
     * True if current item exists
     * */
    virtual bool has() const = 0;

    /*
     * True if next item exists
     * */
    virtual bool hasNext() const = 0;

    /*
     * True if previous item exists
     * */
    virtual bool hasPrev() const = 0;

    /*
     * Takes next element
     * throws exception if no item
     * */
    virtual T next() const = 0;

    /*
     * Takes current element
     * throws exception if no item
     * */
    virtual T cur() const = 0;

    /*
     * Takes previous element
     * throws exception if no item
     * */
    virtual T prev() const = 0;

    /*
     * Returns current cursor position
     * */
    virtual int position() const = 0;

    /*
     * Remove current item and leave cursor at current position
     * throws exception if no item
     * */
    virtual void remove() = 0;

    /*
     * Remove next item and leave cursor at current position
     * throws exception if no item
     * */
    virtual void removeNext() = 0;

    /*
     * Removes previous item and move cursor to previous position (if position > 0)
     * throws exception if no item
     * */
    virtual void removePrev() = 0;

};


#endif //LAB8_ITERATOR_H
