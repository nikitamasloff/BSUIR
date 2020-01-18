using System;

namespace Power
{
    /*
     * Data class representing information about power and battery of computer (e.g. BatteryName, PowerSupplyType ...).
     */
    public class PowerInfo
    {
        public PowerInfo(string batteryDeviceID,
            string batteryName,
            PowerSupplyTypes powerSupplyType,
            PowerSaveModes powerSaveMode,
            uint batteryChargeLevelPercents,
            BatteryChargeStatuses batteryChargeStatus,
            int batteryFullLifeTimeSeconds,
            int batteryLifeRemainingSeconds,
            BatteryChemistryTypes batteryChemistryType)
        {
            BatteryDeviceID = batteryDeviceID;
            BatteryName = batteryName;
            PowerSupplyType = powerSupplyType;
            PowerSaveMode = powerSaveMode;
            BatteryChargeLevelPercents = batteryChargeLevelPercents;
            BatteryChargeStatus = batteryChargeStatus;
            BatteryFullLifeTimeSeconds = batteryFullLifeTimeSeconds;
            BatteryLifeRemainingSeconds = batteryLifeRemainingSeconds;
            BatteryChemistryType = batteryChemistryType;
        }

        /*
         * Device ID of battery.
         */
        public string BatteryDeviceID { get; }

        /*
         * Name of battery.
         */
        public string BatteryName { get; }

        /*
         * Enumeration of power sources.
         */
        public enum PowerSupplyTypes
        {
            /*
             * AC - Alternating Current
             * 
             * Computer is connected to power source.
             */
            AC,

            /*
             * Computer is disconnected from power source, instead it is using battery.
             */
            Battery,

            Unknown
        }

        public PowerSupplyTypes PowerSupplyType { get; }

        /*
         * Enumeration of power save modes.
         */
        public enum PowerSaveModes
        {

            /*
             * The device is not in a power save mode.
             */
            No,

            /*
             * The device is known to be in a power save mode, but its exact status is unknown.
             */
            Unknown,

            /*
             * The device is in a power save state but still functioning, and may exhibit degraded performance.
             */
            LowPowerMode,

            /*
             * The device is not functioning, but could be brought to full power quickly.
             */
            Standby,

            /*
             * The device is in a warning state, though also in a power save mode.
             */
            Warning
        }

        /*
         * Info about current power save mode.
         */
        public PowerSaveModes PowerSaveMode { get; }

        /*
         * Estimated remaining lifetime of battery in percents.
         */
        public uint BatteryChargeLevelPercents { get; }

        /*
         * Additional info about state of battery.
         */
        public enum BatteryChargeStatuses
        {
            Charging,
            Critical,
            High,
            Low,
            NoSystemBattery,
            Unknown
        }
        
        /*
         * Additional info object about battery; one of 'BatteryChargeStatuses'.
         */
        public BatteryChargeStatuses BatteryChargeStatus { get; }

        /*
         * Default value of 'BatteryFullLifeTimeSeconds' in case of invalid value.
         */
        public const int UNKNOWN_BATTERY_FULL_LIFETIME = -1;

        /*
         * Approximate total lifetime of battery in seconds.
         */
        public int BatteryFullLifeTimeSeconds { get; }

        /*
         * Default value of 'BatteryLifeRemainingSeconds' in case of invalid value.
         */
        public const int UNKNOWN_BATTERY_LIFETIME_REMAINING = -1;

        /*
         * Approximate remaining lifetime of battery in seconds.
         */
        public int BatteryLifeRemainingSeconds { get; }

        /*
         * Enumeration of chemical structures of batteries.
         */
        public enum BatteryChemistryTypes
        {
            Other,
            Unknown,
            LeadAcid,
            NickelCadmium,
            NickelMetalHydride,
            LithiumIon,
            ZincAir,
            LithiumPolymer
        }

        /*
         * Info object about battery chemistry type; one of 'BatteryChemistryTypes'.
         */
        public BatteryChemistryTypes BatteryChemistryType { get; }

        /*
         * Pretty representation of object.
         */
        public override string ToString()
        {
            var str =
                "BatteryDeviceID = " + BatteryDeviceID +
                "\n" + "BatteryName = " + BatteryName +
                "\n" + "PowerSupplyType = " + PowerSupplyType.ToString() +
                "\n" + "PowerSaveMode = " + PowerSaveMode.ToString() +
                "\n" + "BatteryChargeLevel = " + BatteryChargeLevelPercents.ToString() + "%" +
                "\n" + "BatteryChargeStatus = " + BatteryChargeStatus;

            if (BatteryFullLifeTimeSeconds != UNKNOWN_BATTERY_FULL_LIFETIME)
            {
                str += "\n" + "BatteryFullLifetime = " + BatteryFullLifeTimeSeconds + "s";
            }

            if (BatteryLifeRemainingSeconds != UNKNOWN_BATTERY_LIFETIME_REMAINING)
            {
                str += "\n" + "BatteryLifeRemaining = " + BatteryLifeRemainingSeconds + "s";
            }

            str += "\n" + "BatteryChemistryType = " + BatteryChemistryType;

            return str;
        }
    }
}
