#include "Parser.h"
#include "List.cpp"

#include <string>
#include <list>
#include <fstream>
#include <istream>

using namespace std;

Parser::Parser() = default;

Parser::~Parser() {
    std::cout << "destructor of 'Parser'\n";
}

void Parser::print() {
    std::cout << "this is 'Parser'\n" << this;
}

void Parser::someFun() {
    std::cout << "some function of 'Parser'\n";
}

void Parser::serialize(const std::list<List<int>> &list, const std::string &fileName, const Parser::Mode &mode) {
    switch (mode) {
        case TEXT_IN_TEXT_MODE:
            return toJson(list, fileName);
        case TEXT_IN_BINARY_MODE:
            return interpret(list, fileName);
        case BINARY_IN_BINARY_MODE:
            return toBinary(list, fileName);
        default:
            throw "unknown Mode";
    }
}

std::list<List<int>> Parser::deserialize(
        const std::string &fileName,
        const Parser::Mode &mode,
        const bool reversedOrder,
        const std::list<std::string> &conditions,
                    const bool deleteFound
                    ) {

    switch (mode) {
        case TEXT_IN_TEXT_MODE:
            return fromJson(fileName,reversedOrder, conditions, deleteFound);
        case TEXT_IN_BINARY_MODE:
            return reinterpret(fileName,reversedOrder, conditions, deleteFound);
        case BINARY_IN_BINARY_MODE:
            return fromBinary(fileName,reversedOrder, conditions, deleteFound);
        default:
            throw "unknown Mode";
    }

}

void Parser::toJson(const std::list<List<int>> &list, const std::string &fileName) {
    ofstream fout(fileName);
    if (!fout.is_open()) {
        cout << '\n' << "error: unable to open a file " << "'" << fileName << "'" << '\n';
        return;
    }
    switch (list.size()) {
        case 0: {
            fout << "{ }";
            break;
        }
        case 1: {
            std::list<List<int>>::iterator it;
            fout << cache(*list.begin(),TEXT_IN_TEXT_MODE);
            break;
        }
        default:{

            fout << '[' << ' ';
            int i = 0;
            auto it = list.begin();

            for (; i < list.size(); ++i) {
                fout << cache(*it++, TEXT_IN_TEXT_MODE);
                if (i != list.size() - 1){
                    fout << ',' << ' ';
                }
            }
            fout << ' ' << ']';

            break;
        }
    }
    fout.close();
}

std::list<List<int>> Parser::fromJson(
        const std::string &fileName,
        const bool reversedOrder,
        const std::list<string> &conditions,
        const bool deleteFound
        ) {

    ifstream fin(fileName);
    std::list<List<int>> accumulator;

    if (!fin.is_open()) {
        cerr << '\n' << "error: unable to open a file " << "'" << fileName << "'" << '\n';
        return accumulator;
    }

    //set file range
    int startFilePos;
    int endFilePos;

    fin.seekg(0, ios::beg);
    startFilePos = (int) fin.tellg();

    fin.seekg(0, ios::end);
    endFilePos = (int) fin.tellg();

    //start reading file
    fin.seekg(startFilePos, ios::beg);

    for (int pos = startFilePos;;) {
        fin.seekg(pos, ios::beg);
        fin.ignore(1024, '{');
        pos = (int)fin.tellg();
        if (pos >= startFilePos && pos < endFilePos){
            auto *temp = fin >> decache(pos - 1, TEXT_IN_TEXT_MODE);
            accumulator.push_back(*temp);
        } else break;
    }

    fin.close();
    return accumulator;
}

void Parser::interpret(const std::list<List<int>> &list, const std::string &fileName) {

    ofstream fout(fileName, ios::binary | ios::out);
    if (!fout.is_open()) {
        cerr << '\n' << "error: unable to open a file " << "'" << fileName << "'" << '\n';
        return;
    }

    auto it = list.begin();
    for (int i = 0; i < list.size(); ++i) {
        fout << cache(*it++, TEXT_IN_BINARY_MODE);
    }

    fout.close();
}

std::list<List<int>> Parser::reinterpret(
        const std::string &fileName,
        bool reversedOrder,
        const std::list<string> &conditions,
        bool deleteFound
        ) {

    std::list<List<int>> accumulator;

    ifstream fin(fileName, ios::binary | ios::in);
    if (!fin.is_open()) {
        cerr << '\n' << "error: unable to open a file " << "'" << fileName << "'" << '\n';
        return accumulator;
    }


    try {
        if (!reversedOrder) {
            fin.seekg(0, ios::beg);
            for (auto *temp = new List<int>;
                 fin.read(reinterpret_cast<char *>(temp), sizeof(List<int>));
                 accumulator.push_back(*temp));
        } else {
            fin.seekg(0, ios::end);
            if ( (int) fin.tellg() == 0) {
                return accumulator;
            }
            fin.seekg( (int) -sizeof(List<int>), ios::end);
            auto *temp = new List<int>;
            while (fin.read(reinterpret_cast<char *>(temp), sizeof(List<int>))) {
                accumulator.push_back(*temp);
                fin.seekg( ((int) -sizeof(List<int>)) * 2, ios::cur);
            }
        }
    }
    catch ( std::string &e) {
        return std::list<List<int>>(0);
    }
    catch ( exception &e) {
        return std::list<List<int>>(0);
    }

    return accumulator;
}

void Parser::toBinary(const std::list<List<int>> &list, const std::string &fileName) {
    ofstream fout(fileName, ios::binary | ios::out);
    if (!fout.is_open()) {
        cerr << '\n' << "error: unable to open file " << "'" << fileName << "'" << '\n';
        return;
    }

    for (const auto &item : list) {
        fout << cache(item, BINARY_IN_BINARY_MODE);
    }

    fout.close();
}

std::list<List<int>>
Parser::fromBinary(
        const std::string &fileName,
        bool reversedOrder,
        const std::list<string> &conditions,
        bool deleteFound
        ) {

    std::list<List<int>> accumulator;

    ifstream fin(fileName, ios::binary | ios::in);
    if (!fin.is_open()) {
        cerr << '\n' << "error: unable to open a file " << "'" << fileName << "'" << '\n';
        return accumulator;
    }

    try {
        if (!reversedOrder) {
            fin.seekg(0, ios::beg);
            for (auto *temp = new List<int>;fin.read((char*)temp, sizeof(List<int>)); ) {
                accumulator.push_back(*temp);
            }
        } else {
            fin.seekg(0, ios::end);
            if ( (int) fin.tellg() == 0) {
                return accumulator;
            }
            fin.seekg( (int) -sizeof(List<int>), ios::end);
            auto *temp = new List<int>;
            while (fin.read((char*)temp, sizeof(List<int>))) {
                accumulator.push_back(*temp);
                fin.seekg( ((int) -sizeof(List<int>)) * 2, ios::cur);
            }
        }
    }
    catch ( std::string &e) {
        return std::list<List<int>>(0);
    }
    catch ( exception &e) {
        return std::list<List<int>>(0);
    }

    return accumulator;
}

Parser::cache::cache(List<int> const &list, Parser::Mode const &mode) : value(list), mode(mode) {}

std::ofstream &operator<<(std::ofstream &fout, Parser::cache cache) {

    auto cacheIntArray = [&](){
        fout << '[' << ' ';
        for (List<int>::ListIterator iterator(const_cast<List<int>&>(cache.value)); iterator.has(); ++iterator) {
            fout << to_string(iterator.cur());
            if (iterator.position() != cache.value.lastIndex()) {
                fout << ',';
            }
        }
        fout << ' ' << ']';
    };

    switch (cache.mode) {
        case Parser::TEXT_IN_TEXT_MODE: {
            fout << '{' << ' ';
            fout << list_constants::FIELD_SIZE << " : " << cache.value.size() << ',' << ' ';
            fout << list_constants::FIELD_ARRAY << " : ";
            cacheIntArray();
            fout << ' ' << '}';
            break;
        }
        case Parser::TEXT_IN_BINARY_MODE: {
            fout.write(reinterpret_cast<char*>(&const_cast<List<int>&>(cache.value)), sizeof(List<int>));
            break;
        }
        case Parser::BINARY_IN_BINARY_MODE: {
            fout.write((char*)&cache.value, sizeof(List<int>));
            break;
        }
        default: throw "unknown mode";
    }
    return fout;
}

Parser::decache::decache(int position, const Parser::Mode &mode) : position(position), mode(mode) {}

List<int> *operator>>(std::ifstream &fin, Parser::decache decache) {

    auto *list = new List<int>;

    switch (decache.mode) {
        case Parser::TEXT_IN_TEXT_MODE: {

            fin.seekg(decache.position, ios::beg);
            fin.setf(ios::skipws);

            //set range of object scope
            int startObjectPos;
            int endObjectPos;

            //position after '{'
            fin.ignore(1024, '{');
            startObjectPos = (int) fin.tellg();

            //position before '}'
            fin.ignore(1024, '}');
            endObjectPos = (int) fin.tellg();
            --endObjectPos;

            //start reading object
            fin.seekg(startObjectPos, ios::beg);

            std::string buffer;

            while ( (int)fin.tellg() >= startObjectPos && (int)fin.tellg() < endObjectPos) {

                fin >> buffer;

                if (buffer == list_constants::FIELD_SIZE) {
                    fin.ignore(1024, ':');
                    int size;
                    fin >> size;
                    cout << "read size = " << size << '\n';
                    continue;
                }
                if (buffer == list_constants::FIELD_ARRAY) {

                    fin.ignore(1024, ':');

                    //set range of int array scope
                    int startIntArrayPos;
                    int endIntArrayPos;

                    //position after '['
                    fin.ignore(1024, '[');
                    startIntArrayPos = (int) fin.tellg();

                    //position before ']'
                    fin.ignore(1024, ']');
                    endIntArrayPos = (int) fin.tellg();
                    --endIntArrayPos;

                    //end of file
                    fin.seekg(0, ios::end);
                    int endOfFilePos = (int) fin.tellg();

                    //start reading int array
                    fin.seekg(startIntArrayPos, ios::beg);

                    int number;
                    std::list<int> intList;

                    while (((int)fin.tellg()) >= startIntArrayPos && ((int) fin.tellg()) < endIntArrayPos) {
                        fin >> number;
                        intList.push_back(number);
                        fin.ignore(1024, ',');
                    }

                    cout << "read array = " << '[';
                    for (auto item : intList) {
                        cout << ' ' << item << ' ';
                    }
                    cout << ']' << '\n';

                    for (auto item : intList) {
                        list->add(item);
                    }

                    continue;
                }
            }
            break;
        }
        case Parser::TEXT_IN_BINARY_MODE: {

            fin.seekg(decache.position, ios::beg);
            fin.read(reinterpret_cast<char*>(list), sizeof(List<int>));

            break;
        }
        case Parser::BINARY_IN_BINARY_MODE: {

            fin.seekg(decache.position, ios::beg);
            fin.read((char*)list, sizeof(List<int>));

            break;
        }
        default:
            throw "unknown_mode";
    }

    return list;
}

ostream &operator<<(ostream &os, const Parser &parser) {
    os << parser << " someProtectedField: " << parser.someProtectedField
       << " somePrivateField: " << parser.somePrivateField << '\n';
    return os;
}
