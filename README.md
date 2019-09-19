# labcpp8-12

class 'List' based on static array with standard functionality, inner iterator, proxy to have both get[] and set[] operators,
some overloaded operators;

implemented compatibility with std::list and std::sort;

class 'Parser' to (de)serialize 'List' instance to file and back;
3 types of serialization: toJson/fromJson(text file in text mode), using reinterpret_cast(text file in binary mode) and using C-style cast(binary file in binary mode);
for (de)serization overloaded parameterized operators, basic fstream operators(tellg, seekp), fstream flags(skipws) etc. were used;
implemented default deserialization and reversed(reading from end of file);
some console logs during deserialization;

class 'Object' is used as parent class (12 lab), inheritors: List, Parser, SomeClass (dynamic constructors, unique functionality)

all demonstrative tests in main;
