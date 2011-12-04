#if !LOCALTEST

namespace System {

	public sealed class Version : ICloneable, IComparable, IComparable<Version>, IEquatable<Version> {
		int _Major, _Minor, _Build, _Revision;

		private const int UNDEFINED = -1;

		private void CheckedSet(int defined, int major, int minor, int build, int revision) {
			// defined should be 2, 3 or 4

			if (major < 0) {
				throw new ArgumentOutOfRangeException("major");
			}
			this._Major = major;

			if (minor < 0) {
				throw new ArgumentOutOfRangeException("minor");
			}
			this._Minor = minor;

			if (defined == 2) {
				this._Build = UNDEFINED;
				this._Revision = UNDEFINED;
				return;
			}

			if (build < 0) {
				throw new ArgumentOutOfRangeException("build");
			}
			this._Build = build;

			if (defined == 3) {
				this._Revision = UNDEFINED;
				return;
			}

			if (revision < 0) {
				throw new ArgumentOutOfRangeException("revision");
			}
			this._Revision = revision;
		}

		public Version() {
			CheckedSet(2, 0, 0, -1, -1);
		}

		public Version(string version) {
			int n;
			string[] vals;
			int major = -1, minor = -1, build = -1, revision = -1;

			if (version == null) {
				throw new ArgumentNullException("version");
			}

			vals = version.Split('.');
			n = vals.Length;

			if (n < 2 || n > 4) {
				throw new ArgumentException("There must be 2, 3 or 4 components in the version string.");
			}

			if (n > 0) {
				major = int.Parse(vals[0]);
			}
			if (n > 1) {
				minor = int.Parse(vals[1]);
			}
			if (n > 2) {
				build = int.Parse(vals[2]);
			}
			if (n > 3) {
				revision = int.Parse(vals[3]);
			}

			CheckedSet(n, major, minor, build, revision);
		}

		public Version(int major, int minor) {
			CheckedSet(2, major, minor, 0, 0);
		}

		public Version(int major, int minor, int build) {
			CheckedSet(3, major, minor, build, 0);
		}

		public Version(int major, int minor, int build, int revision) {
			CheckedSet(4, major, minor, build, revision);
		}

		public int Build {
			get {
				return _Build;
			}
		}

		public int Major {
			get {
				return _Major;
			}
		}

		public int Minor {
			get {
				return _Minor;
			}
		}

		public int Revision {
			get {
				return _Revision;
			}
		}

		public short MajorRevision {
			get {
				return (short)(_Revision >> 16);
			}
		}

		public short MinorRevision {
			get {
				return (short)_Revision;
			}
		}

		public object Clone() {
			if (_Build == -1) {
				return new Version(_Major, _Minor);
			} else if (_Revision == -1) {
				return new Version(_Major, _Minor, _Build);
			} else {
				return new Version(_Major, _Minor, _Build, _Revision);
			}
		}

		public int CompareTo(object version) {
			if (version == null) {
				return 1;
			}
			if (!(version is Version)) {
				throw new ArgumentException("Argument to Version.CompareTo must be a Version.");
			}
			return this.CompareTo((Version)version);
		}

		public override bool Equals(object obj) {
			return this.Equals(obj as Version);
		}

		public int CompareTo(Version v) {
			if (v == null) {
				return 1;
			}
			if (this._Major > v._Major) {
				return 1;
			} else if (this._Major < v._Major) {
				return -1;
			}
			if (this._Minor > v._Minor) {
				return 1;
			} else if (this._Minor < v._Minor) {
				return -1;
			}
			if (this._Build > v._Build) {
				return 1;
			} else if (this._Build < v._Build) {
				return -1;
			}
			if (this._Revision > v._Revision) {
				return 1;
			} else if (this._Revision < v._Revision) {
				return -1;
			}
			return 0;
		}

		public bool Equals(Version x) {
			return ((x != null) &&
				(x._Major == _Major) &&
				(x._Minor == _Minor) &&
				(x._Build == _Build) &&
				(x._Revision == _Revision));
		}

		public override int GetHashCode() {
			return (_Revision << 24) | (_Build << 16) | (_Minor << 8) | _Major;
		}

		// <summary>
		//   Returns a stringified representation of the version, format:
		//   major.minor[.build[.revision]]
		// </summary>
		public override string ToString() {
			string mm = _Major.ToString() + "." + _Minor.ToString();

			if (_Build != UNDEFINED) {
				mm = mm + "." + _Build.ToString();
			}
			if (_Revision != UNDEFINED) {
				mm = mm + "." + _Revision.ToString();
			}

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
		public string ToString(int fields) {
			if (fields == 0) {
				return String.Empty;
			}
			if (fields == 1) {
				return _Major.ToString();
			}
			if (fields == 2) {
				return _Major.ToString() + "." + _Minor.ToString();
			}
			if (fields == 3) {
				if (_Build == UNDEFINED) {
					throw new ArgumentException
						("fields is larger than the number of components defined in this instance.");
				}
				return _Major.ToString() + "." + _Minor.ToString() + "." + _Build.ToString();
			}
			if (fields == 4) {
				if (_Build == UNDEFINED || _Revision == UNDEFINED) {
					throw new ArgumentException
						("fields is larger than the number of components defined in this instance.");
				}
				return _Major.ToString() + "." + _Minor.ToString() + "." + _Build.ToString() + "." + _Revision.ToString();
			}
			throw new ArgumentException("Invalid fields parameter: " + fields.ToString());
		}

		public static bool operator ==(Version v1, Version v2) {
			return Equals(v1, v2);
		}

		public static bool operator !=(Version v1, Version v2) {
			return !Equals(v1, v2);
		}

		public static bool operator >(Version v1, Version v2) {
			return v1.CompareTo(v2) > 0;
		}

		public static bool operator >=(Version v1, Version v2) {
			return v1.CompareTo(v2) >= 0;
		}

		public static bool operator <(Version v1, Version v2) {
			return v1.CompareTo(v2) < 0;
		}

		public static bool operator <=(Version v1, Version v2) {
			return v1.CompareTo(v2) <= 0;
		}

		// a very gentle way to construct a Version object which takes 
		// the first four numbers in a string as the version
		internal static Version CreateFromString(string info) {
			int major = 0;
			int minor = 0;
			int build = 0;
			int revision = 0;
			int state = 1;
			int number = UNDEFINED; // string may not begin with a digit

			if (info == null) {
				return new Version(0, 0, 0, 0);
			}
			for (int i = 0; i < info.Length; i++) {
				char c = info[i];
				if (char.IsDigit(c)) {
					if (number < 0) {
						number = (c - '0');
					} else {
						number = (number * 10) + (c - '0');
					}
				} else if (number >= 0) {
					// assign
					switch (state) {
						case 1:
							major = number;
							break;
						case 2:
							minor = number;
							break;
						case 3:
							build = number;
							break;
						case 4:
							revision = number;
							break;
					}
					number = -1;
					state++;
				}
				// ignore end of string
				if (state == 5)
					break;
			}

			// Last number
			if (number >= 0) {
				switch (state) {
					case 1:
						major = number;
						break;
					case 2:
						minor = number;
						break;
					case 3:
						build = number;
						break;
					case 4:
						revision = number;
						break;
				}
			}
			return new Version(major, minor, build, revision);
		}
	}
}

#endif
