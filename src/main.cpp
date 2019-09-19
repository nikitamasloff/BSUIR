#include <iostream>
#include "COM.h"

using namespace std;

int main() {
    COM com;
    com.connect("COM1", 9600);

    for (int i = 0; i < 1000; ++i) {
        uint8_t buf = 0;
        int buf_size = 6;
        com.read(&buf, buf_size);
        std::cout << buf << endl;
    }

    system("PAUSE");
    return EXIT_SUCCESS;
}
