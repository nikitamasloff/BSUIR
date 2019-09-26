namespace HDD
{
    class Memory
    {

        public const uint MB = 1024 * 1024;
        private Memory(ulong total, ulong used)
        {
            Total = total;
            Used = used;
        }

        public static Memory FromTotalAndUsed(ulong total, ulong used)
        {
            return new Memory(total, used);
        }

        public static Memory FromTotalAndFree(ulong total, ulong free)
        {
            return new Memory(total, total - free);
        }

        public ulong Total { get; }
        public ulong Used { get; }
        public ulong Free
        {
            get
            {
                return Total - Used;
            }
        }

        public override string ToString()
        {
            return "Total = " + Total / MB + "Mb, Used = " + Used / MB + "Mb, Free = " + Free / MB + "Mb";
        }
    }
}
