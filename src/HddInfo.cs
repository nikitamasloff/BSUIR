using System.Collections.Generic;

namespace HDD
{
    class HddInfo
    {
        public HddInfo(string name,
                       string model,
                       string manufacturer,
                       string serialNumber,
                       string firmwareVersion,
                       Memory memory,
                       string interfaceType,
                       List<string> supportedModes)
        {
            Name = name;
            Model = model;
            Manufacturer = manufacturer;
            SerialNumber = serialNumber;
            FirmwareVersion = firmwareVersion;
            Memory = memory;
            InterfaceType = interfaceType;
            SupportedModes = supportedModes;
        }

        public string Name { get; }
        public string Model { get; }
        public string Manufacturer { get; }
        public string SerialNumber { get; }
        public string FirmwareVersion { get; }
        public Memory Memory { get; }
        public string InterfaceType { get; }
        public List<string> SupportedModes { get; }

        public override string ToString()
        {
            return "Name = " + Name + ",\nModel = " + Model + ",\nManufacturer = " + Manufacturer
                + ",\nSerialNumber = " + SerialNumber + ",\nFirmwareVersion = " + FirmwareVersion
                + ",\nMemory = { " + Memory.ToString() + " },\nInterfaceType = " + InterfaceType
                + ",\nSupportedModes = [ " + string.Join(",", SupportedModes.ToArray()) + " ]";
        }
    }
}
