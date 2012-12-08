namespace System
{
    public sealed class Version : ICloneable, IComparable, IComparable<Version>, IEquatable<Version>
    {
        private const int UNDEFINED = -1;

        private int mMajor;
        private int mMinor;
        private int mBuild;
        private int mRevision;

        private void CheckedSet(int defined, int major, int minor, int build, int revision)
        {
            // defined should be 2, 3 or 4

            if (major < 0) throw new ArgumentOutOfRangeException("major");
            mMajor = major;

            if (minor < 0) throw new ArgumentOutOfRangeException("minor");
            mMinor = minor;

            if (defined == 2)
            {
                mBuild = UNDEFINED;
                mRevision = UNDEFINED;
                return;
            }

            if (build < 0) throw new ArgumentOutOfRangeException("build");
            mBuild = build;

            if (defined == 3)
            {
                mRevision = UNDEFINED;
                return;
            }

            if (revision < 0) throw new ArgumentOutOfRangeException("revision");
            mRevision = revision;
        }

        public Version()
        {
            CheckedSet(2, 0, 0, -1, -1);
        }

        public Version(string version)
        {
            int n;
            string[] vals;
            int major = -1, minor = -1, build = -1, revision = -1;

            if (version == null) throw new ArgumentNullException("version");

            vals = version.Split('.');
            n = vals.Length;

            if (n < 2 || n > 4) throw new ArgumentException("There must be 2, 3 or 4 components in the version string.");

            if (n > 0) major = int.Parse(vals[0]);
            if (n > 1) minor = int.Parse(vals[1]);
            if (n > 2) build = int.Parse(vals[2]);
            if (n > 3) revision = int.Parse(vals[3]);

            CheckedSet(n, major, minor, build, revision);
        }

        public Version(int major, int minor)
        {
            CheckedSet(2, major, minor, 0, 0);
        }

        public Version(int major, int minor, int build)
        {
            CheckedSet(3, major, minor, build, 0);
        }

        public Version(int major, int minor, int build, int revision)
        {
            CheckedSet(4, major, minor, build, revision);
        }

        public int Build { get { return mBuild; } }

        public int Major { get { return mMajor; } }

        public int Minor { get { return mMinor; } }

        public int Revision { get { return mRevision; } }

        public short MajorRevision { get { return (short)(mRevision >> 16); } }

        public short MinorRevision { get { return (short)mRevision; } }

        public object Clone()
        {
            if (mBuild == -1) return new Version(mMajor, mMinor);
            if (mRevision == -1) return new Version(mMajor, mMinor, mBuild);
            return new Version(mMajor, mMinor, mBuild, mRevision);
        }

        public int CompareTo(object version)
        {
            if (version == null) return 1;
            if (!(version is Version)) throw new ArgumentException("Argument to Version.CompareTo must be a Version.");
            return CompareTo((Version)version);
        }

        public override bool Equals(object obj) { return Equals(obj as Version); }

        public int CompareTo(Version v)
        {
            if (v == null) return 1;
            if (mMajor > v.mMajor) return 1;
            if (mMajor < v.mMajor) return -1;
            if (mMinor > v.mMinor) return 1;
            if (mMinor < v.mMinor) return -1;
            if (mBuild > v.mBuild) return 1;
            if (mBuild < v.mBuild) return -1;
            if (mRevision > v.mRevision) return 1;
            if (mRevision < v.mRevision) return -1;
            return 0;
        }

        public bool Equals(Version x) { return x != null && x.mMajor == mMajor && x.mMinor == mMinor && x.mBuild == mBuild && x.mRevision == mRevision; }

        public override int GetHashCode() { return (mRevision << 24) | (mBuild << 16) | (mMinor << 8) | mMajor; }

        // <summary>
        //   Returns a stringified representation of the version, format:
        //   major.minor[.build[.revision]]
        // </summary>
        public override string ToString()
        {
            string mm = mMajor.ToString() + "." + mMinor.ToString();
            if (mBuild != UNDEFINED) mm = mm + "." + mBuild.ToString();
            if (mRevision != UNDEFINED) mm = mm + "." + mRevision.ToString();
            return mm;
        }

        // <summary>
        //    LAME: This API is lame, since there is no way of knowing
        //    how many fields a Version object has, it is unfair to throw
        //    an ArgumentException, but this is what the spec claims.
        //
        //    ie, Version a = new Version (1, 2);  a.ToString (3) should
        //    throw the expcetion.
        // </summary>
        public string ToString(int fields)
        {
            if (fields == 0) return String.Empty;
            if (fields == 1) return mMajor.ToString();
            if (fields == 2) return mMajor.ToString() + "." + mMinor.ToString();
            if (fields == 3)
            {
                if (mBuild == UNDEFINED) throw new ArgumentException("fields is larger than the number of components defined in this instance.");
                return mMajor.ToString() + "." + mMinor.ToString() + "." + mBuild.ToString();
            }
            if (fields == 4)
            {
                if (mBuild == UNDEFINED || mRevision == UNDEFINED) throw new ArgumentException("fields is larger than the number of components defined in this instance.");
                return mMajor.ToString() + "." + mMinor.ToString() + "." + mBuild.ToString() + "." + mRevision.ToString();
            }
            throw new ArgumentException("Invalid fields parameter: " + fields.ToString());
        }

        public static bool operator ==(Version v1, Version v2) { return Equals(v1, v2); }

        public static bool operator !=(Version v1, Version v2) { return !Equals(v1, v2); }

        public static bool operator >(Version v1, Version v2) { return v1.CompareTo(v2) > 0; }

        public static bool operator >=(Version v1, Version v2) { return v1.CompareTo(v2) >= 0; }

        public static bool operator <(Version v1, Version v2) { return v1.CompareTo(v2) < 0; }

        public static bool operator <=(Version v1, Version v2) { return v1.CompareTo(v2) <= 0; }

        // a very gentle way to construct a Version object which takes 
        // the first four numbers in a string as the version
        internal static Version CreateFromString(string info)
        {
            int major = 0;
            int minor = 0;
            int build = 0;
            int revision = 0;
            int state = 1;
            int number = UNDEFINED; // string may not begin with a digit

            if (info == null) return new Version(0, 0, 0, 0);
            for (int i = 0; i < info.Length; i++)
            {
                char c = info[i];
                if (char.IsDigit(c))
                {
                    if (number < 0) number = (c - '0');
                    else number = (number * 10) + (c - '0');
                }
                else if (number >= 0)
                {
                    // assign
                    switch (state)
                    {
                        case 1: major = number; break;
                        case 2: minor = number; break;
                        case 3: build = number; break;
                        case 4: revision = number; break;
                        default: break;
                    }
                    number = -1;
                    state++;
                }
                // ignore end of string
                if (state == 5) break;
            }

            // Last number
            if (number >= 0)
            {
                switch (state)
                {
                    case 1: major = number; break;
                    case 2: minor = number; break;
                    case 3: build = number; break;
                    case 4: revision = number; break;
                    default: break;
                }
            }
            return new Version(major, minor, build, revision);
        }
    }
}
