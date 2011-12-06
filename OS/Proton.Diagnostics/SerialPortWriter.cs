using System;

namespace Proton.Diagnostics
{
    public static class SerialPortWriter
    {
        private static Proton.Hardware.SerialPort sSerialPort = null;

        public static void SetSerialPort(Proton.Hardware.SerialPort pSerialPort)
        {
            sSerialPort = pSerialPort;
            Console.WriteLine("Problem Here: pSerialPort " + (pSerialPort == null ? "==" : "!=") + " null, sSerialPort " + (sSerialPort == null ? "==" : "!=") + " null");
        }

        public static void WriteString(string pString)
        {
            if (sSerialPort == null) return;
            for (int index = 0; index < pString.Length; ++index) sSerialPort.WriteByte((byte)pString[index]);
        }

        public static void WriteLine(string pLine) { WriteString(pLine + "\r\n"); }
    }
}
