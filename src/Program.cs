using System;

namespace HDD
{
    class Program
    {
        static void Main(string[] args)
        {
            HddInfoProvider hddInfoProvider = new HddInfoProvider();
            foreach (HddInfo hddInfo in hddInfoProvider.GetHddInfos())
            {
                Console.WriteLine(hddInfo.ToString());
            }
        }
    }
}
