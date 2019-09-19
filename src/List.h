#ifndef LAB8_LIST_H
#define LAB8_LIST_H

#include "Container.h"
#include "Iterator.h"
#include "Constant.h"

#include <iostream>
#include <functional>
#include <string>
#include <fstream>
#include <list>

template <typename T = int> class List : public Container<T> {

public:

    List();
    virtual ~List() override;
    void print() override;

    explicit List(int size);
    List(int size, std::function<T(int index)> init);

    T get(int index) const override;
    T operator[](int index) const override;

    void add(T item) override;
    void add(T item, int index) override;
    void addAll(std::list<T>);

    void clear() override;

    bool contains(T item) const override;

    int indexOf(T item) const override;

    int lastIndex() const override;

    int size() const override;

    bool isEmpty() const override;

    void remove(int index) override;

    List<T> *copy() const;

    bool operator==(List<T> t) const;
    bool operator!=(List<T> t)const;

    explicit operator std::list<T>() const;

    /*
     * Filter persisted elements, leave only suitable
     * */
    List<T> *filter(std::function<bool(T)> block) const;

    /*
     * Map all persisted items to given type
     * */
    template <typename To = std::string> List<To> *map(std::function<To(T)> block) const;

    /*
     * Returns first element that satisfies condition
     * throws exception if no element was found
     * */
    T find(std::function<bool(T)>);

    template <typename T1> friend std::ostream &operator<<(std::ostream &os, const List<T1> &list);

    enum SORT_MODE {
        ASCENDING,
        DESCENDING
    };

    void sort(SORT_MODE);

    /*
     * Proxy for differentiation get[] and set[] operators
     * */
    class Proxy {

    public:

        Proxy(List<T> &list, int index);
        T operator()(int index);
        void operator[](const T &value);

    private:
        List<T> &list;
        int index;

    };

    Proxy operator[](int index);


    class ListIterator : public Iterator<T> {

    public:

        explicit ListIterator(List<T>  &list);

        bool has() const override;
        bool hasNext() const override;
        bool hasPrev() const override;
        T next() const override;
        T cur() const override;
        T prev() const override;
        int position() const override;
        void remove() override;
        void removeNext() override;
        void removePrev() override;

        /*
         * Move cursor to next item, returns iterator with changed cursor
         * */
        ListIterator operator++();

        /*
         * Move cursor to next item, returns iterator with unchanged cursor
         * */
        const ListIterator operator++(int);

        /*
         * Move cursor to previous item, returns iterator with changed cursor
         * */
        ListIterator operator--();

        /*
         * Move cursor to previous item, returns iterator with unchanged cursor
         * */
        const ListIterator operator--(int);

    private:

        /*
         * Current position of cursor
         * */
        int cursor = 0;

        /*
         * Linked list
         * */
        List<T> &list;

    };

    ListIterator iterator();

private:

    T array[list_constants::AVAILABLE_SIZE] {};
    int _size = 0;

    bool isIndexExist(int index, bool throwIfException = false) const;

};

#endif //LAB8_LIST_H
