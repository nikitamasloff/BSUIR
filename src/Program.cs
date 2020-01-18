using System;
using System.Windows.Forms;

namespace Power
{
    class Program
    {
        static void Main(string[] args)
        {

            /*
             * Auto disposable.
             */
            using (PowerManager powerManager = new PowerManager())
            {
                //PowerManager.Suspend();

                powerManager.ObservePowerInfo(powerInfo =>
                {
                    Console.WriteLine("\n" + powerInfo.ToString() + "\n");
                }, /*Log every 5 sec*/ 5_000, 
                triggerOnSystemEvents: true);

                // Suspend program termination for 200 seconds.
                Wait(200_000);
            }
        }

        /*
         * Suspend program termination.
         */
        private static void Wait(uint delayMillis)
        {
            DateTime after = DateTime.Now.AddMilliseconds(delayMillis);
            while (DateTime.Now < after)
            {
                Application.DoEvents();
            }
        }
    }
}
