#pragma once

#include <string>
#include <ostream>

/*
 * Data class to represent single device.
 */
class DeviceInfo {

public:
    DeviceInfo(std::string t_name,
            std::string t_vendorId,
            std::string t_deviceId);

private:
    std::string m_name;
    std::string m_vendorId;
    std::string m_deviceId;

public:
    const std::string &getName() const;

    void setName(const std::string &t_name);

    const std::string &getVendorId() const;

    void setVendorId(const std::string &t_vendorId);

    const std::string &getDeviceId() const;

    void setDeviceId(const std::string &t_deviceId);

    friend std::ostream &operator<<(std::ostream &os, const DeviceInfo &info);
};
