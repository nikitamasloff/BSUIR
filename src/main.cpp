#include <iostream>
#include "DeviceProvider.h"

/*
 * https://stackoverflow.com/questions/33640791/setupapi-setupdi-functions-dont-link
 * https://social.msdn.microsoft.com/Forums/vstudio/en-US/e14ce1a2-08b9-4291-b035-f72171bcbdbc/error-lnk2019-unresolved-external-symbol?forum=vclanguage
 */
#pragma comment (lib, "setupapi.lib")

int main() {
    std::cout << "Hello, World!" << std::endl;

    DeviceProvider provider;
    if (!provider.open()) {
        exit(EXIT_FAILURE);
    }

    auto devices = provider.getDevices();
    for (const DeviceInfo& deviceInfo : devices) {
        std::cout << deviceInfo << std::endl;
    }

    provider.close();
    system("pause");
    return EXIT_SUCCESS;
}