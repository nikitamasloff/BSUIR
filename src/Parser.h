#ifndef LAB8_PARSER_H
#define LAB8_PARSER_H

#include "List.h"
#include "Object.h"

#include <string>
#include <list>
#include <vector>
#include <ostream>

class Parser : public Object {

public:

    Parser();
    virtual ~Parser() override;
    void print() override;

    enum Mode {
        TEXT_IN_TEXT_MODE,
        TEXT_IN_BINARY_MODE,
        BINARY_IN_BINARY_MODE
    };

    /*
     * Serialize to file / deserialize from file
     *
     * Conditions { |fieldName1 = value1|, |fieldName2 = value2|, ... }
     * */
    void serialize(const std::list<List<int>> &list, const std::string &fileName, const Mode &mode);
    std::list<List<int>> deserialize(
            const std::string &fileName,
            const Mode &mode,
            bool reversedOrder = false,
            const std::list<std::string> &conditions = std::list<std::string>(0),
            bool deleteFound = false
                    );

    /*
     * std::ofstream parameterized operator for caching 'List<int>' to stream
     * */
    class cache {
    public:
        explicit cache(const List<int> &list, const Mode &mode);
        friend std::ofstream &operator<<(std::ofstream&, cache);
    private:
        const List<int> &value;
        const Mode &mode;
    };

    /*
     * std::ifstream parameterized operator for decaching 'List<int>' from stream
     * */
    class decache {
    public:
        explicit decache(int position, const Mode &mode);
        friend List<int> *operator>>(std::ifstream&, decache);
    private:
        const int position;
        const Mode &mode;
    };

    void someFun();
    friend std::ostream &operator<<(std::ostream &os, const Parser &parser);

protected:
    int someProtectedField = 2;

private:
    int somePrivateField = 1;

    /*
     * (De)Serialize with 'JSON'
     * */
    void toJson(const std::list<List<int>> &list, const std::string &fileName);
    std::list<List<int>> fromJson(
            const std::string &fileName,
            bool reversedOrder = false,
            const std::list<std::string> &conditions = std::list<std::string>(0),
            bool deleteFound = false
            );

    /*
     * (De)Serialize with 'reinterpret_cast'
     * */
    void interpret(const std::list<List<int>> &list, const std::string &fileName);
    std::list<List<int>> reinterpret(
            const std::string &fileName,
            bool reversedOrder = false,
            const std::list<std::string> &conditions = std::list<std::string>(0),
            bool deleteFound = false
            );

    /*
     * (De)Serialize as blocks of memory
     * */
    void toBinary(const std::list<List<int>> &list, const std::string &fileName);
    std::list<List<int>> fromBinary(
            const std::string &fileName,
            bool reversedOrder = false,
            const std::list<std::string> &conditions = std::list<std::string>(0),
            bool deleteFound = false
            );

};


#endif //LAB8_PARSER_H
