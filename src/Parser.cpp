#include "Parser.h"

DeviceInfo Parser::parse(const std::string &t_nameSrc, const std::string &t_idsSrc) {
    const std::string &name = t_nameSrc;
    std::string vendorId = t_idsSrc.substr(
            t_idsSrc.find("VEN_") + 4,
            t_idsSrc.find("&DEV") - t_idsSrc.find("VEN_") - 4
    );
    std::string deviceId = t_idsSrc.substr(
            t_idsSrc.find("DEV_") + 4,
            t_idsSrc.find("&SUB") - t_idsSrc.find("DEV_") - 4
    );
    return DeviceInfo(name, vendorId, deviceId);
}
