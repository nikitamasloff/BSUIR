#pragma once

#include "DeviceInfo.h"

class Parser {

public:

    /*
     * Parses DeviceInfo from source strings.
     */
    static DeviceInfo parse(const std::string &t_nameSrc, const std::string &t_idsSrc);

};
