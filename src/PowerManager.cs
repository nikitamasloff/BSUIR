using Microsoft.Win32;
using System;
using System.Management;
using System.Windows.Forms;

namespace Power
{
    /*
     * Class to access info about power of computer.
     */
    public class PowerManager : IDisposable
    {
        public PowerManager() { }

        /*
         * Handler linked to battery observing events.
         * It is cleaned in 'Dispose' to avoid leaks.
         */
        private PowerModeChangedEventHandler handler = null;

        /*
         * Query to access 'Win32_Battery' class via SQL statement.
         */
        private const string WIN32_BATTERY_QUERY = "SELECT * FROM Win32_Battery";

        /*
         * Handler linked to the battery info query.
         */
        private ManagementObjectSearcher _searcher = null;

        /*
         * Wrap '_searcher' as property to initialize if null;
         */
        private ManagementObjectSearcher searcher
        {
            get
            {
                if (_searcher == null)
                {
                    _searcher = new ManagementObjectSearcher(WIN32_BATTERY_QUERY);
                }
                return _searcher;
            }
            set
            {
                _searcher = value;
            }
        }

        /*
         * Handler to access additional info about power and battery.
         */
        private PowerStatus _powerStatus = null;

        /*
         * Wrap '_powerStatus' as property to initialize if null;
         */
        private PowerStatus powerStatus
        {
            get
            {
                if (_powerStatus == null)
                {
                    _powerStatus = SystemInformation.PowerStatus;
                }
                return _powerStatus;
            }
            set
            {
                _powerStatus = value;
            }
        }

        /*
         * Reference to timer object to avoid leaks.
         */
        private Timer timer = null;

        /*
         * Get info about power of computer, returning custom object 'PowerInfo'.
         */
        public PowerInfo GetPowerInfo()
        {
            var batteryDeviceId = GetBatteryDeviceID();
            var batteryName = GetBatteryName();
            var powerSupplyType = GetPowerSupplyType();
            var powerSaveMode = GetPowerSaveMode();
            var batteryChargeLevelPercents = GetBatteryChargeLevelPercents();
            var batteryChargeStatus = GetBatteryChargeStatus();
            var batteryFullLifetimeSeconds = GetBatteryFullLifeTimeSeconds();
            var batteryLifeRemainingSeconds = GetBatteryLifeRemainingSeconds();
            var batteryChemistryType = GetBatteryChemistryType();

            return new PowerInfo(
                batteryDeviceId,
                batteryName,
                powerSupplyType,
                powerSaveMode,
                batteryChargeLevelPercents,
                batteryChargeStatus,
                batteryFullLifetimeSeconds,
                batteryLifeRemainingSeconds,
                batteryChemistryType
                );
        }

        /*
         * Reference: https://docs.microsoft.com/en-us/dotnet/api/microsoft.win32.systemevents.powermodechanged
         * 
         * Observe information about power of computer.
         * Triggers passed callback every time power event is happening (e.g. Turn On/Off power, switch to standby mode ...).
         * 
         * NOTE: Calling 'Dispose' or passing caller object in 'using' statement will clear handler,
         * so callback will be called as long as caller object is not being cleared. 
         */
        public void ObservePowerInfo(PowerInfoDelegate callback)
        {
            this.handler = CreatePowerModeChangedEventHandler(this, callback);
            SystemEvents.PowerModeChanged += this.handler;
        }

        /*
         * Reference: https://docs.microsoft.com/en-us/dotnet/api/microsoft.win32.systemevents.powermodechanged
         * 
         * Observe information about power of computer.
         * Triggers passed callback every 'delayMillis' milliseconds.
         * If 'triggerOnSystemEvents' set to 'true'
         * then triggers callback every time power event is happening (e.g. Turn On/Off power, switch to standby mode ...).
         * 
         * NOTE: Calling 'Dispose' or passing caller object in 'using' statement will clear handler,
         * so callback will be called as long as caller object is not being cleared. 
         */
        public void ObservePowerInfo(PowerInfoDelegate callback, int delayMillis, bool triggerOnSystemEvents = true)
        {
            if (delayMillis < 0)
            {
                throw new Exception("Invalid delay value");
            }

            if (triggerOnSystemEvents)
            {
                ObservePowerInfo(callback);
            }

            Timer timer = new Timer();
            timer.Interval = delayMillis;
            timer.Tick += (sender, args) =>
            {
                var powerInfo = GetPowerInfo();
                callback(powerInfo);
            };
            timer.Start();

            if (this.timer != null)
            {
                this.timer.Dispose();
            }
            this.timer = timer;
        }

        /*
         * Execute finalization actions: close handlers and another sensitive resources.
         */
        public void Dispose()
        {
            if (this.handler != null)
            {
                // Remove handler to avoid leaks.
                SystemEvents.PowerModeChanged -= this.handler;
            }

            if (this._searcher != null)
            {
                // Close DB access handler.
                this._searcher.Dispose();
            }

            if (this.timer != null)
            {
                // Dispose timer.
                this.timer.Dispose();
            }

            _powerStatus = null;
        }

        /*
         * Utility method to wrap 'PowerInfoDelegate' with system handler.
         */
        private static PowerModeChangedEventHandler CreatePowerModeChangedEventHandler(
            PowerManager powerManager, 
            PowerInfoDelegate callback)
        {
            return new PowerModeChangedEventHandler((sender, args) =>
            {
                var powerInfo = powerManager.GetPowerInfo();
                callback(powerInfo);
            });
        }

        /*
         * Reference: https://docs.microsoft.com/en-us/dotnet/api/system.windows.forms.powerstatus.powerlinestatus
         * 
         * Detects whether computer is connected to AC or is using battery.
         */
        public PowerInfo.PowerSupplyTypes GetPowerSupplyType()
        {
            switch (this.powerStatus.PowerLineStatus)
            {
                case (PowerLineStatus.Online): return PowerInfo.PowerSupplyTypes.AC;
                case (PowerLineStatus.Offline): return PowerInfo.PowerSupplyTypes.Battery;
                default: return PowerInfo.PowerSupplyTypes.Unknown;
            }
        }

        /*
         * Reference: https://docs.microsoft.com/en-us/windows/win32/cimwin32prov/win32-battery
         * 
         * Returns current mode of power save (e.g. LowPowerMode, Standby ...).
         */
        public PowerInfo.PowerSaveModes GetPowerSaveMode()
        {
            foreach (var obj in this.searcher.Get())
            {
                // Return first one only.
                switch (Convert.ToUInt16(obj.Properties["Availability"].Value))
                {
                    case (13): return PowerInfo.PowerSaveModes.Unknown;
                    case (14): return PowerInfo.PowerSaveModes.LowPowerMode;
                    case (15): return PowerInfo.PowerSaveModes.Standby;
                    case (17): return PowerInfo.PowerSaveModes.Warning;
                    default: return PowerInfo.PowerSaveModes.No;
                }
            }

            // Default result.
            return PowerInfo.PowerSaveModes.No;
        }

        /*
         * Reference: https://docs.microsoft.com/en-us/dotnet/api/system.windows.forms.powerstatus.batterylifepercent
         * 
         * Returns charge level of battery in percents.
         */
        public uint GetBatteryChargeLevelPercents()
        {
            return (uint)Math.Round(this.powerStatus.BatteryLifePercent * 100);
        }

        /*
         * Reference: https://docs.microsoft.com/en-us/dotnet/api/system.windows.forms.batterychargestatus
         * 
         * Returns info about battery status: whether it is charging, low on power ...
         */
        public PowerInfo.BatteryChargeStatuses GetBatteryChargeStatus()
        {
            switch (this.powerStatus.BatteryChargeStatus)
            {
                case (BatteryChargeStatus.Charging): return PowerInfo.BatteryChargeStatuses.Charging;
                case (BatteryChargeStatus.Critical): return PowerInfo.BatteryChargeStatuses.Critical;
                case (BatteryChargeStatus.High): return PowerInfo.BatteryChargeStatuses.High;
                case (BatteryChargeStatus.Low): return PowerInfo.BatteryChargeStatuses.Low;
                case (BatteryChargeStatus.NoSystemBattery): return PowerInfo.BatteryChargeStatuses.NoSystemBattery;
                default: return PowerInfo.BatteryChargeStatuses.Unknown;
            }
        }

        /*
         * Reference: https://docs.microsoft.com/en-us/dotnet/api/system.windows.forms.powerstatus.batteryfulllifetime
         * 
         * Returns total estimated lifetime of battery in seconds 
         * or 'PowerInfo.UNKNOWN_BATTERY_FULL_LIFETIME' in case of invalid value.
         */
        public int GetBatteryFullLifeTimeSeconds()
        {
            var value = this.powerStatus.BatteryFullLifetime;
            return value > 0 ? value : PowerInfo.UNKNOWN_BATTERY_FULL_LIFETIME;
        }

        /*
         * Reference: https://docs.microsoft.com/en-us/dotnet/api/system.windows.forms.powerstatus.batteryliferemaining
         * 
         * Returns remaining estimated lifetime of battery in seconds 
         * or 'PowerInfo.UNKNOWN_BATTERY_FULL_LIFETIME' in case of invalid value.
         */
        public int GetBatteryLifeRemainingSeconds()
        {
            var value = this.powerStatus.BatteryLifeRemaining;
            return value > 0 ? value : PowerInfo.UNKNOWN_BATTERY_LIFETIME_REMAINING;
        }

        /*
         * Reference: https://docs.microsoft.com/en-us/windows/win32/cimwin32prov/win32-battery
         * 
         * Returns object representing chemistry type of battery (e.g. LeadAcid, NickelCadmium, LithiumIon ...).
         */
        public PowerInfo.BatteryChemistryTypes GetBatteryChemistryType()
        {
            foreach (var obj in this.searcher.Get())
            {
                // Return first one only.
                switch (Convert.ToUInt16(obj.Properties["Chemistry"].Value))
                {
                    case (1): return PowerInfo.BatteryChemistryTypes.Other;
                    case (2): return PowerInfo.BatteryChemistryTypes.Unknown;
                    case (3): return PowerInfo.BatteryChemistryTypes.LeadAcid;
                    case (4): return PowerInfo.BatteryChemistryTypes.NickelCadmium;
                    case (5): return PowerInfo.BatteryChemistryTypes.NickelMetalHydride;
                    case (6): return PowerInfo.BatteryChemistryTypes.LithiumIon;
                    case (7): return PowerInfo.BatteryChemistryTypes.ZincAir;
                    case (8): return PowerInfo.BatteryChemistryTypes.LithiumPolymer;
                    default: return PowerInfo.BatteryChemistryTypes.Unknown;
                }
            }

            // Default result.
            return PowerInfo.BatteryChemistryTypes.Unknown;
        }

        /*
         * Reference: https://docs.microsoft.com/en-us/windows/win32/cimwin32prov/win32-battery
         * 
         * Returns device ID of current battery.
         */
        public string GetBatteryDeviceID()
        {
            foreach (var obj in this.searcher.Get())
            {
                // Return first one only.
                return Convert.ToString(obj.Properties["DeviceID"].Value);
            }

            // Default result.
            return "";
        }

        /*
         * Reference: https://docs.microsoft.com/en-us/windows/win32/cimwin32prov/win32-battery
         * 
         * Returns name of current battery.
         */
        public string GetBatteryName()
        {
            foreach (var obj in this.searcher.Get())
            {
                // Return first one only.
                return Convert.ToString(obj.Properties["Name"].Value);
            }

            // Default result.
            return "";
        }

        /*
         * Reference: https://docs.microsoft.com/en-us/dotnet/api/system.windows.forms.application.setsuspendstate
         * 
         * Switch the OS to sleep (standby) mode.
         */
        public static void Suspend()
        {
            Application.SetSuspendState(PowerState.Suspend, true, true);
        }

        /*
         * Reference: https://docs.microsoft.com/en-us/dotnet/api/system.windows.forms.application.setsuspendstate
         * 
         * Switch the OS to hibernate mode.
         */
        public static void Hibernate()
        {
            Application.SetSuspendState(PowerState.Hibernate, true, true);
        }
    }

    /*
     * Lambda to observe power events (e.g. Turn On/Off power, switch to standby mode ...).
     */
    public delegate void PowerInfoDelegate(PowerInfo powerInfo);
}
