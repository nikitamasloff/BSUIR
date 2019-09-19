#include "List.h"
#include "List.cpp"
#include "Parser.h"
#include "SomeClass.h"

#include <iostream>
#include <string>
#include <functional>
#include <list>
#include <sstream>
#include <assert.h>

using namespace std;

void test();
std::string bool_to_string(bool);

int main() {
    test();
    return 0;
}

void test() {

    /*
     * List from lambda
     * */
    cout << "init 'List<int>' with lambda" << '\n';
    List<int> list = List<int>(10, [](int index){
        return index * index;
    });
    cout << list;


    /*
     * List of chars
     * */
    cout << "explicit 'char*' specialization (List<char*>)" << '\n';
    List<char*> charList = List<char*>(10, [](int index){
        char *ch = new char;
        *ch = static_cast<char>('0' + index);
        return ch;
    });
    cout << charList;


    /*
     * Default template type argument template<typename T = int>, empty list of zeroes
     * */
    cout << "default template type argument" << '\n';
    List<> intList = List<>(22);
    cout << intList;


    /*
     * Serialize to file and then deserialize back
     * */
    Parser parser;
    List<int> serializeList1 = List<int>(10, [](int index) {
        return index * index;
    });
    List<int> serializeList2 = List<int>(15, [](int index) {
        return index % 2 ? index : -index;
    });

    const string fileNameTextInTextMode = "text_in_text_mode.txt";
    const string fileNameTextInBinaryMode = "text_in_binary_mode.txt";
    const string fileNameBinaryInBinaryMode = "binary_in_binary_mode.bin";

    const bool reversedTextInTextMode = true;
    const bool reversedTextInBinaryMode = true;
    const bool reversedBinaryInBinaryMode = true;

    cout << "lists for serialization: " << '\n';
    cout << serializeList1;
    cout << serializeList2;


    /*
     * Text file in text mode
     * */
    cout << "TEXT file in TEXT mode (fileName = text_in_text_mode.txt)\nreversed = " + bool_to_string(reversedTextInTextMode) << '\n';
    parser.serialize(
            std::list<List<int>>({serializeList1, serializeList2}),
            fileNameTextInTextMode,
            Parser::TEXT_IN_TEXT_MODE);
    auto deserialize1 = parser.deserialize(
            fileNameTextInTextMode,
            Parser::TEXT_IN_TEXT_MODE,
            reversedTextInTextMode,
            {"_size = 10"},
            true
    );

    /*
     * Check if deserialized lists equals serialized
     * */
    cout << "deserialize1:\n";
    cout << deserialize1.front();
    cout << deserialize1.back();
    assert(deserialize1.front() == serializeList1);
    assert(deserialize1.back() == serializeList2);


    /*
     * Text file in binary mode
     * */

    cout << "TEXT file in BINARY mode (fileName = text_in_binary_mode.txt)\nreversed = " + bool_to_string(reversedTextInBinaryMode) << '\n';
    parser.serialize(
            std::list<List<int>>({serializeList1, serializeList2}),
            fileNameTextInBinaryMode,
            Parser::TEXT_IN_BINARY_MODE);
    auto deserialize2 = parser.deserialize(
            fileNameTextInBinaryMode,
            Parser::TEXT_IN_BINARY_MODE,
            reversedTextInBinaryMode,
            {"_size = 10"},
            true);

    /*
     * Check if deserialized lists equals serialized
     * */
    cout << "deserialize2:\n";
    cout << deserialize2.front();
    cout << deserialize2.back();
    assert(deserialize2.front() == serializeList2);
    assert(deserialize2.back() == serializeList1);


    /*
     * Binary file in binary mode
     * */

    cout << "BINARY file in BINARY mode (fileName = binary_in_binary_mode.txt)\nreversed = " + bool_to_string(reversedBinaryInBinaryMode) << '\n';
    parser.serialize(
            std::list<List<int>>({serializeList1, serializeList2}),
            fileNameBinaryInBinaryMode,
            Parser::BINARY_IN_BINARY_MODE);
    auto deserialize3 = parser.deserialize(
            fileNameBinaryInBinaryMode,
            Parser::BINARY_IN_BINARY_MODE,
            reversedBinaryInBinaryMode,
            {"_size = 10"},
            true);

    /*
     * Check if deserialized lists equals serialized
     * */
    cout << "deserialize3:\n";
    cout << deserialize3.front();
    cout << deserialize3.back();
    assert(deserialize3.front() == serializeList2);
    assert(deserialize3.back() == serializeList1);




    /*
     * Basic add,remove,filer, e.t.c. operations
     * */
    cout << "list before 'add'\n";
    cout << list;
    list.add(5);
    list.add(6);
    cout << "list after 'add'\n";
    cout << list;

    cout << "list before 'remove'\n";
    cout << list;
    list.remove(1);
    list.remove(2);
    cout << "list2 after 'remove'\n";
    cout << list;


    /*
    * Test polymorphism
    * */
    const int arraySize = 4;

    auto **array = new Object*[arraySize];

    auto *objectPointer = new Object();
    auto *listPointer = new List<int>(10);
    auto *parserPointer = new Parser();
    auto *someClassPointer = new SomeClass();

    array[0] = objectPointer;
    array[1] = listPointer;
    array[2] = parserPointer;
    array[3] = someClassPointer;

    for (int i = 0; i < arraySize; ++i) {
        auto *objectTemp = dynamic_cast<Object*>(array[i]);
        auto *listTemp = dynamic_cast<List<int>*>(array[i]);
        auto parserTemp = dynamic_cast<Parser*>(array[i]);
        auto *someClass = dynamic_cast<SomeClass*>(array[i]);

        if (objectTemp) {
            (*objectTemp).print();
        }
        if (listTemp) {
            (*listTemp).print();
            cout << "list size = " << (*listTemp).size() << '\n';
        }
        if (parserTemp) {
            (*parserTemp).print();
            (*parserTemp).someFun();
        }
        if (someClass) {
            (*someClass).print();
            (*someClass).someFun();
        }

    }

    for (int j = 0; j < arraySize; ++j) {
        delete array[j];
    }

    delete[] array;

}

std::string bool_to_string(bool value) {
    return value ? "true" : "false";
}