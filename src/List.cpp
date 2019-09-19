#include "List.h"

#include <string>
#include <functional>

using namespace std;

template<typename T>
List<T>::List() : _size(0) {};

template<typename T>
List<T>::~List() {
    std::cout << "destructor of 'List'\n";
}

template<typename T>
void List<T>::print() {
    std::cout << "this is 'List'\n" << this;
}

template<typename T>
List<T>::List(int size) : _size(size) {}

template<typename T>
List<T>::List(int size, function<T(int)> init) : _size(size) {
    for (int i = 0; i < this->_size; ++i) {
        this->array[i] = init(i);
    }
}

template <typename T>
bool List<T>::isIndexExist(int index, bool throwIfException) const {
    bool isValid = index >= 0 && index < _size;
    if (throwIfException && !isValid) {
        cerr << "invalid index";
        throw "invalid_index";
    }
    return isValid;
}

template <typename T>
bool List<T>::isEmpty() const {
    return _size == 0;
}

template <typename T>
int List<T>::indexOf(T item) const {
    for (int i = 0; i < _size; ++i) {
        if (array[i] == item) {
            return i;
        }
    }
    return -1;
}

template <typename T>
int List<T>::lastIndex() const {
    return  _size > 0 ? _size - 1 : 0;
}

template<typename T>
List<T> *List<T>::copy() const {
    auto *list = new List<T>();
    for (int i = 0; i < this->_size; ++i) {
        list->add(this->array[i]);
    }
    return list;
}

template <typename T>
List<T> *List<T>::filter(function<bool(T)> block) const {
    List<T> *list = this->copy();
    for (int i = 0; i < this->_size; ++i) {
        auto item = this->array[i];
        if (block(item)){
            list->add(item);
        }
    }
    return list;
}

template <typename From>
template <typename To>
List<To> *List<From>::map(function<To(From src)> block) const {
    return new List<To>(_size, [this, block](int index)->To{
        return block(array[index]);
    });
}

template<typename T>
T List<T>::find(std::function<bool(T)> block) {
    for (int i = 0; i < this->_size; ++i) {
        if (block(array[i])){
            return array[i];
        }
    }
    throw "no_item_found";
}

template<typename T>
T List<T>::get(int index) const {
    isIndexExist(index, true);
    return this->array[index];
}

template<typename T>
T List<T>::operator[](int index) const {
    return get(index);
}

template<typename T>
void List<T>::add(T item) {
    add(item, _size);
}

template<typename T>
void List<T>::add(T item, int index) {
    bool pushBack = index == _size || isIndexExist(index, true);
    if (pushBack) {
        this->array[_size++] = item;
        return;
    }
    T temp = this->array[index];
    for (int i = _size; i > index; --i) {
        this->array[i] = this->array[i-1];
    }
    this->array[index] = temp;
    this->_size++;
}

template<typename T>
void List<T>::addAll(std::list<T> list) {
    for (auto item : list) {
        this->add(item);
    }
}

template<typename T>
void List<T>::clear() {
    this->_size = 0;
}

template<typename T>
bool List<T>::contains(T item) const {
    for (T innerItem : this->array){
        if (innerItem == item) return true;
    }
    return false;
}

template<typename T>
void List<T>::remove(int index) {
    isIndexExist(index, true);
    if (isEmpty()) return;
    for (int i = index; i < _size - 1; ++i) {
        this->array[i] = this->array[i+1];
    }
    this->_size--;
}

template<typename T>
int List<T>::size() const {
    return _size;
}

template <typename T>
std::ostream& operator<<(std::ostream &os, const List<T> &list) {
    string temp;
    for (int i = 0; i < list._size; ++i) {
        temp += to_string(list.array[i]);
        if (i != list._size - 1) temp += ',';
    }
    return os << "size = " << list._size << ", array = " + temp << '\n';
}

template < >
inline std::ostream& operator<<(std::ostream &os, const List<char*> &list) {
    string temp;
    for (int i = 0; i < list._size; ++i) {
        temp += *list.array[i];
        if (i != list._size - 1) temp += ',';
    }
    return os << "(explicit 'char*' specialization) " << "size = " << list._size << ", array = " + temp << '\n';
}

template<typename T>
bool List<T>::operator==(List<T> t) const {
    if (this->_size != t._size) return false;
    for (int i = 0; i < this->_size; ++i) {
        if (this->array[i] != t.array[i]) return false;
    }
    return true;
}

template<typename T>
bool List<T>::operator!=(List<T> t) const {
    return !(*this == t);
}

template<typename T>
List<T>::operator std::list<T>() const {
    std::list<T> temp;
    for (int i = 0; i < this->_size; ++i) {
        temp.push_back(array[i]);
    }
    return temp;
}

template<typename T>
typename List<T>::ListIterator List<T>::iterator() {
    return List::ListIterator(*this);
}

template<typename T>
typename List<T>::Proxy List<T>::operator[](int index) {
    return List::Proxy(*this, index);
}

template<typename T>
void List<T>::sort(SORT_MODE sortMode) {
    auto temp = (std::list<T>) *this;
    this->clear();
    switch (sortMode) {
        case ASCENDING: {
            temp.sort();
            break;
        }
        case DESCENDING: {
            temp.sort(std::greater<T>());
            break;
        }
    }
    this->addAll(temp);
}


/*
 * Iterator implementation
 * */

template<typename T>
List<T>::ListIterator::ListIterator(List<T> &list) : list(list) {}

template<typename T>
bool List<T>::ListIterator::has() const {
    return cursor < list._size;
}

template<typename T>
bool List<T>::ListIterator::hasNext() const {
    return (cursor + 1) < list._size;
}

template<typename T>
bool List<T>::ListIterator::hasPrev()  const {
    return (cursor - 1) < list._size && (cursor - 1) >= 0;
}

template<typename T>
T List<T>::ListIterator::next() const {
    if (hasNext()) return list.array[cursor+1];
    else throw "no next";
}

template <typename T>
T List<T>::ListIterator::cur() const {
    if (has()) return list.array[cursor];
    else throw "no cur";
}

template<typename T>
T List<T>::ListIterator::prev() const {
    if (hasPrev()) return list.array[cursor-1];
    else throw "no prev";
}

template<typename T>
int List<T>::ListIterator::position() const {
    return cursor;
}

template<typename T>
void List<T>::ListIterator::remove() {
    if (has()) {
        this->list.remove(cursor);
    }
    else throw "no cur";
}

template<typename T>
void List<T>::ListIterator::removeNext() {
    if (hasNext()) {
        this->list.remove(cursor+1);
    }
    else throw "no next";
}

template<typename T>
void List<T>::ListIterator::removePrev() {
    if (hasPrev()) {
        this->list.remove(cursor-1);
        operator--();
    }
    else throw "no prev";
}

template<typename T>
typename List<T>::ListIterator List<T>::ListIterator::operator++() {
    cursor++;
    return *this;
}

template<typename T>
const typename List<T>::ListIterator List<T>::ListIterator::operator++(int) {
    ListIterator temp = *this;
    this->operator++();
    return temp;
}

template<typename T>
typename List<T>::ListIterator List<T>::ListIterator::operator--() {
    if (cursor != 0) cursor--;
    return *this;
}

template<typename T>
const typename List<T>::ListIterator List<T>::ListIterator::operator--(int) {
    ListIterator temp = *this;
    this->operator--();
    return temp;
}


template<typename T>
List<T>::Proxy::Proxy(List<T> &list, int index) : list(list), index(index) {}

template<typename T>
T List<T>::Proxy::operator()(int index) {
    return this->list.get(index);
}

template<typename T>
void List<T>::Proxy::operator[](const T &value) {
    this->list.add(value, this->index);
}
