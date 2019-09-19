#include "DeviceInfo.h"

#include <utility>

DeviceInfo::DeviceInfo(std::string t_name,
        std::string t_vendorId,
        std::string t_deviceId)
        : m_name(std::move(t_name)),
        m_vendorId(std::move(t_vendorId)),
        m_deviceId(std::move(t_deviceId)) {}

const std::string &DeviceInfo::getName() const {
    return m_name;
}

void DeviceInfo::setName(const std::string &t_name) {
    DeviceInfo::m_name = t_name;
}

const std::string &DeviceInfo::getVendorId() const {
    return m_vendorId;
}

void DeviceInfo::setVendorId(const std::string &t_vendorId) {
    DeviceInfo::m_vendorId = t_vendorId;
}

const std::string &DeviceInfo::getDeviceId() const {
    return m_deviceId;
}

void DeviceInfo::setDeviceId(const std::string &t_deviceId) {
    DeviceInfo::m_deviceId = t_deviceId;
}

std::ostream &operator<<(std::ostream &os, const DeviceInfo &info) {
    os << "name: " << info.m_name;
    os << "  |  vendor id: " << info.m_vendorId;
    os << "  |  device id: " << info.m_deviceId;
    return os;
}
