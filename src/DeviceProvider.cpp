#include "DeviceProvider.h"
#include "Parser.h"

DeviceProvider::DeviceProvider() : isActive(false), m_devInfoSet(nullptr) {}

DeviceProvider::~DeviceProvider() {
    if (isActive) {
        close();
    }
}

bool DeviceProvider::open() {

    isActive = true;

    /*
     * https://docs.microsoft.com/en-us/windows/win32/api/setupapi/nf-setupapi-setupdigetclassdevsw
     *
     * The SetupDiGetClassDevs function returns a handle to a device information set
     * that contains requested device information elements for a local computer.
     *
     *
     * WINSETUPAPI HDEVINFO SetupDiGetClassDevsW(
     *      const GUID *ClassGuid,
     *      PCWSTR     Enumerator,
     *      HWND       hwndParent,
     *      DWORD      Flags
     * );
     *
     *
     * ClassGuid
     *
     * A pointer to the GUID for a device setup class or a device interface class.
     * This pointer is optional and can be NULL.
     *
     *
     * Enumerator
     *
     * A pointer to a NULL-terminated string that specifies:
     *
     * An identifier (ID) of a Plug and Play (PnP) enumerator.
     * This ID can either be the value's globally unique identifier (GUID) or symbolic name.
     * For example, "PCI" can be used to specify the PCI PnP value.
     * Other examples of symbolic names for PnP values include "USB," "PCMCIA," and "SCSI".
     *
     *
     * hwndParent
     *
     * A handle to the top-level window to be used for a user interface
     * that is associated with installing a device instance in the device information set.
     * This handle is optional and can be NULL.
     *
     *
     * Flags
     *
     * A variable of type DWORD that specifies control options that filter the device information elements
     * that are added to the device information set.
     * This parameter can be a bitwise OR of zero or more of the following flags.
     *
     * DIGCF_ALLCLASSES
     * Return a list of installed devices for all device setup classes or all device interface classes.
     *
     * DIGCF_PRESENT
     * Return only devices that are currently present in a system.
     *
     */
    HDEVINFO handle = SetupDiGetClassDevs(
            nullptr,
            "PCI",
            nullptr,
            DIGCF_ALLCLASSES | DIGCF_PRESENT);

    if (handle == INVALID_HANDLE_VALUE) {
        return false;
    } else {
        this->m_devInfoSet = handle;
		isActive = true;
        return true;
    }
}

bool DeviceProvider::close() {

    isActive = false;

    /*
     * https://docs.microsoft.com/en-us/windows/win32/api/setupapi/nf-setupapi-setupdidestroydeviceinfolist
     *
     * The SetupDiDestroyDeviceInfoList function deletes a device information set and frees all associated memory.
     *
     *
     * WINSETUPAPI BOOL SetupDiDestroyDeviceInfoList(
     *      HDEVINFO DeviceInfoSet
     * );
     *
     * DeviceInfoSet
     *
     * A handle to the device information set to delete.
     *
     *
     * The function returns TRUE if it is successful.
     * Otherwise, it returns FALSE and the logged error can be retrieved with a call to GetLastError.
     */
    return SetupDiDestroyDeviceInfoList(m_devInfoSet);
}

std::vector<DeviceInfo> DeviceProvider::getDevices() {

    /*
     * https://docs.microsoft.com/en-us/windows/win32/api/setupapi/ns-setupapi-sp_devinfo_data
     *
     * An SP_DEVINFO_DATA structure defines a device instance that is a member of a device information set.
     * An SP_DEVINFO_DATA structure identifies a device in a device information set.
     *
     *
     * typedef struct _SP_DEVINFO_DATA {
     *      DWORD     cbSize;
     *      GUID      ClassGuid;
     *      DWORD     DevInst;
     *      ULONG_PTR Reserved;
     * } SP_DEVINFO_DATA, *PSP_DEVINFO_DATA;
     *
     *
     * cbSize
     *
     * The size, in bytes, of the SP_DEVINFO_DATA structure.
     */
    SP_DEVINFO_DATA devInfoData;

    /*
     * Since the caller of 'SetupDiEnumDeviceInfo' must set DeviceInfoData.cbSize to sizeof(SP_DEVINFO_DATA)
     */
    devInfoData.cbSize = sizeof(devInfoData);

    std::vector<DeviceInfo> accumulator;
    int counter = 0;

    /*
     * https://docs.microsoft.com/en-us/windows/win32/api/setupapi/nf-setupapi-setupdienumdeviceinfo
     *
     * The SetupDiEnumDeviceInfo function returns a SP_DEVINFO_DATA structure
     * that specifies a device information element in a device information set.
     *
     *
     * WINSETUPAPI BOOL SetupDiEnumDeviceInfo(
     *      HDEVINFO         DeviceInfoSet,
     *      DWORD            MemberIndex,
     *      SP_DEVINFO_DATA DeviceInfoData
     * );
     *
     *
     * DeviceInfoSet
     *
     * A handle to the device information set for which to return an SP_DEVINFO_DATA structure
     * that represents a device information element.
     *
     *
     * MemberIndex
     *
     * A zero-based index of the device information element to retrieve.
     *
     *
     * DeviceInfoData
     *
     * A pointer to an SP_DEVINFO_DATA structure to receive information about an enumerated device information element.
     * The caller must set DeviceInfoData.cbSize to sizeof(SP_DEVINFO_DATA).
     *
     *
     * The function returns TRUE if it is successful.
     * Otherwise, it returns FALSE and the logged error can be retrieved with a call to GetLastError.
     */
    while (SetupDiEnumDeviceInfo(m_devInfoSet, counter++, &devInfoData)) {

        const int BUFFER_SIZE = 1024;
        byte *nameBuf = new byte[BUFFER_SIZE];
        byte *idsBuf = new byte[BUFFER_SIZE];

        getDeviceName(devInfoData, &nameBuf, BUFFER_SIZE);
        getDeviceIds(devInfoData, &idsBuf, BUFFER_SIZE);

        std::string name = std::string(reinterpret_cast<char *>(nameBuf));
        std::string ids = std::string(reinterpret_cast<char *>(idsBuf), BUFFER_SIZE);

        DeviceInfo devInfo = Parser::parse(name, ids);
        accumulator.push_back(devInfo);

        //delete[] nameBuf;
        //delete[] idsBuf;
    }

    return accumulator;
}

bool DeviceProvider::getDeviceName(SP_DEVINFO_DATA &devInfoData, byte **buffer, int bufferSize) {

    /*
     * https://docs.microsoft.com/en-us/windows/win32/api/setupapi/nf-setupapi-setupdigetdeviceregistrypropertya
     *
     * The SetupDiGetDeviceRegistryProperty function retrieves a specified Plug and Play device property.
     *
     *
     * WINSETUPAPI BOOL SetupDiGetDeviceRegistryPropertyA(
     *      HDEVINFO         DeviceInfoSet,
     *      PSP_DEVINFO_DATA DeviceInfoData,
     *      DWORD            Property,
     *      PDWORD           PropertyRegDataType,
     *      PBYTE            PropertyBuffer,
     *      DWORD            PropertyBufferSize,
     *      PDWORD           RequiredSize
     * );
     *
     *
     *
     * DeviceInfoSet
     *
     * A handle to a device information set that contains a device information element that represents the device for which to retrieve a Plug and Play property.
     *
     *
     * DeviceInfoData
     *
     * A pointer to an SP_DEVINFO_DATA structure that specifies the device information element in DeviceInfoSet.
     *
     *
     * Property
     *
     * One of the following values that specifies the property to be retrieved:
     *
     * SPDRP_HARDWAREID
     * The function retrieves a REG_MULTI_SZ string that contains the list of hardware IDs for a device.
     *
     * SPDRP_DEVICEDESC
     * The function retrieves a REG_SZ string that contains the description of a device.
     *
     *
     * PropertyRegDataType
     *
     * A pointer to a variable that receives the data type of the property that is being retrieved.
     * This is one of the standard registry data types. This parameter is optional and can be NULL.
     *
     *
     * PropertyBuffer
     *
     * A pointer to a buffer that receives the property that is being retrieved.
     * If this parameter is set to NULL, and PropertyBufferSize is also set to zero,
     * the function returns the required size for the buffer in RequiredSize.
     *
     *
     * PropertyBufferSize
     * The size, in bytes, of the PropertyBuffer buffer.
     *
     *
     * RequiredSize
     *
     * A pointer to a variable of type DWORD that receives the required size, in bytes,
     * of the PropertyBuffer buffer that is required to hold the data for the requested property.
     * This parameter is optional and can be NULL.
     *
     *
     * SetupDiGetDeviceRegistryProperty returns TRUE if the call was successful.
     * Otherwise, it returns FALSE and the logged error can be retrieved by making a call to GetLastError.
     */
    return SetupDiGetDeviceRegistryProperty(
            m_devInfoSet,
            &devInfoData,
            SPDRP_DEVICEDESC,
            nullptr,
            (BYTE *) *buffer,
            bufferSize,
            nullptr);
}

bool DeviceProvider::getDeviceIds(SP_DEVINFO_DATA &devInfoData, byte **buffer, int bufferSize) {
    return SetupDiGetDeviceRegistryProperty(
            m_devInfoSet,
            &devInfoData,
            SPDRP_HARDWAREID,
            nullptr,
            (BYTE *) *buffer,
            bufferSize,
            nullptr);
}
