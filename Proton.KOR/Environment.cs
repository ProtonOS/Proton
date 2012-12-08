using System.Runtime.CompilerServices;

namespace System
{
    public static class Environment
    {
        public static string NewLine { get { return (Platform == PlatformID.Unix) ? "\n" : "\r\n"; } }

        public extern static int TickCount
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;
        }

        private static OperatingSystem os = null;

        internal extern static PlatformID Platform
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static string GetOSVersionString();

        public static OperatingSystem OSVersion
        {
            get
            {
                if (os == null)
                {
                    Version v = Version.CreateFromString(GetOSVersionString());
                    PlatformID p = Platform;
                    os = new OperatingSystem(p, v);
                }
                return os;
            }
        }

        internal static bool IsRunningOnWindows
        {
            get
            {
                return Platform != PlatformID.Unix;
            }
        }

    }
}
