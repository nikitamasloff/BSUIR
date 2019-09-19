#pragma once

#include "DeviceInfo.h"

#include <windows.h>
#include <setupapi.h>
#include <vector>

/*
 * Class to access device information set
 *
 * Based on:
 * https://docs.microsoft.com/en-us/windows-hardware/drivers/install/enumerating-installed-device-interface-classes
 */
class DeviceProvider {

private:

    bool isActive;

    /*
     * Handle to access devices info.
     */
    HDEVINFO m_devInfoSet;

    /*
     * Get device name by 'devInfoData'.
     * Result is saved in buffer.
     *
     * Returns 'true' if successful, 'false' if error.
     */
    bool getDeviceName(SP_DEVINFO_DATA &devInfoData, byte **buffer, int bufferSize);

    /*
     * Get device ids by 'devInfoData'.
     * Result is saved in buffer.
     *
     * Returns 'true' if successful, 'false' if error.
     */
    bool getDeviceIds(SP_DEVINFO_DATA &devInfoData, byte **buffer, int bufferSize);

public:

    DeviceProvider();
    ~DeviceProvider();

    /*
     * Initializes member handle to a device information set.
     *
     * Returns 'true' if successful, 'false' if error.
     */
    bool open();

    /*
     * Destroys device information set by member handle.
     *
     * Returns 'true' if successful, 'false' if error.
     */
    bool close();

    /*
     * Get PSI device information set.
     */
    std::vector<DeviceInfo> getDevices();
};
