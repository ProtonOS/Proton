using Proton.Devices;
using System;


namespace Proton.Diagnostics
{
    public static class SerialWriter
    {
        private static Serial sSerial = null;

        public static void SetSerial(Serial pSerial) { sSerial = pSerial; }

        public static void WriteString(string pString)
        {
            if (sSerial == null) return;
            for (int index = 0; index < pString.Length; ++index) sSerial.WriteByte((byte)pString[index]);
        }

        public static void WriteLine(string pLine) { WriteString(pLine + "\r\n"); }
    }
}
