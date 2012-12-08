namespace System
{
    public sealed class OperatingSystem : ICloneable
    {
        private PlatformID mPlatformID;
        private Version mVersion;

        public OperatingSystem(PlatformID platformID, Version version)
        {
            if (version == null)
            {
                throw new ArgumentNullException("version");
            }
            mPlatformID = platformID;
            mVersion = version;
        }

        public PlatformID Platform { get { return mPlatformID; } }

        public Version Version { get { return mVersion; } }

        public string ServicePack { get { return String.Empty; } }

        public string VersionString { get { return ToString(); } }

        public override string ToString()
        {
            string str;

            switch (mPlatformID)
            {
                case PlatformID.Win32NT:
                    str = "Microsoft Windows NT";
                    break;
                case PlatformID.Win32S:
                    str = "Microsoft Win32S";
                    break;
                case PlatformID.Win32Windows:
                    str = "Microsoft Windows 98";
                    break;
                case PlatformID.WinCE:
                    str = "Microsoft Windows CE";
                    break;
                case PlatformID.Unix:
                    str = "Unix";
                    break;
                default:
                    str = "<unknown>";
                    break;
            }

            return str + " " + mVersion.ToString() + " (Fusion)";
        }

        #region ICloneable Members

        public object Clone()
        {
            return (OperatingSystem)object.MemberwiseClone(this);
        }

        #endregion
    }
}
