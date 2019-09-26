using System;
using System.Collections.Generic;
using System.Management;
using System.Text;

namespace HDD
{
    // https://docs.microsoft.com/en-us/windows/win32/cimwin32prov/win32-diskdrive
    class HddInfoProvider
    {
        public List<HddInfo> GetHddInfos()
        {
            var driveQuery = new ManagementObjectSearcher("SELECT * FROM Win32_DiskDrive");

            var accumulator = new List<HddInfo>();
            foreach (ManagementObject drive in driveQuery.Get())
            {
                var physicalName = Convert.ToString(drive.Properties["Name"].Value); // \\.\PHYSICALDRIVE2
                var diskModel = Convert.ToString(drive.Properties["Model"].Value); // WDC WD5001AALS-xxxxxx
                var diskInterface = Convert.ToString(drive.Properties["InterfaceType"].Value); // IDE
                var manufacturer = Convert.ToString(drive.Properties["Manufacturer"].Value);
                var firmwareVersion = Convert.ToString(drive.Properties["FirmwareRevision"].Value);
                var serialNumber = Convert.ToString(drive.Properties["SerialNumber"].Value);
                var totalSpace = Convert.ToUInt64(drive.Properties["Size"].Value);

                //var capabilities = (UInt16[])drive.Properties["Capabilities"].Value;
                var capabilityDescriptions = (string[])drive.Properties["CapabilityDescriptions"].Value;
                var supportedModes = new List<string>();
                foreach (string capability in capabilityDescriptions)
                {
                    supportedModes.Add(capability);
                }

                ulong usedSpace = 0;
                var partitionQueryText = string.Format("ASSOCIATORS OF {{{0}}} WHERE AssocClass = Win32_DiskDriveToDiskPartition", drive.Path.RelativePath);
                var partitionQuery = new ManagementObjectSearcher(partitionQueryText);
                foreach (ManagementObject partition in partitionQuery.Get())
                {
                    var logicalDriveQueryText = string.Format("ASSOCIATORS OF {{{0}}} WHERE AssocClass = Win32_LogicalDiskToPartition", partition.Path.RelativePath);
                    var logicalDriveQuery = new ManagementObjectSearcher(logicalDriveQueryText);

                    foreach (ManagementObject logicalDrive in logicalDriveQuery.Get())
                    {
                        var logicalDriveFreeSpace = Convert.ToUInt64(logicalDrive.Properties["FreeSpace"].Value); // in bytes
                        var logicalDriveTotalSpace = Convert.ToUInt64(logicalDrive.Properties["Size"].Value); // in bytes
                        var logicalDriveUsedSpace = logicalDriveTotalSpace - logicalDriveFreeSpace;
                        usedSpace += logicalDriveUsedSpace;
                    }
                }

                var memory = Memory.FromTotalAndUsed(totalSpace, usedSpace);

                var hddInfo = new HddInfo(physicalName,
                                                  diskModel,
                                                  manufacturer,
                                                  serialNumber,
                                                  firmwareVersion,
                                                  memory,
                                                  diskInterface,
                                                  supportedModes);
                accumulator.Add(hddInfo);
            }
            driveQuery.Dispose();
            return accumulator;
        }
    }
}
