#if !LOCALTEST

using System.Globalization;
namespace System {
	public struct Double : IFormattable, IComparable, IComparable<double>, IEquatable<double> {

		public const double Epsilon = 4.9406564584124650e-324;
		public const double MaxValue = 1.7976931348623157e308;
		public const double MinValue = -1.7976931348623157e308;
		public const double NaN = 0.0d / 0.0d;
		public const double NegativeInfinity = -1.0d / 0.0d;
		public const double PositiveInfinity = 1.0d / 0.0d;

		internal double m_value;

		public static bool IsNaN(double d) {
#pragma warning disable 1718
			return d != d;
#pragma warning restore
		}

		public static bool IsNegativeInfinity(double d) {
			return (d < 0.0d && (d == NegativeInfinity || d == PositiveInfinity));
		}

		public static bool IsPositiveInfinity(double d) {
			return (d > 0.0d && (d == NegativeInfinity || d == PositiveInfinity));
		}

		public static bool IsInfinity(double d) {
			return (d == PositiveInfinity || d == NegativeInfinity);
		}

		public override bool Equals(object o) {
			if (!(o is System.Double)) {
				return false;
			}
			if (IsNaN((double)o)) {
				return IsNaN(this.m_value);
			}
			return ((double)o) == this.m_value;
		}

		public override unsafe int GetHashCode() {
			double d = m_value;
			return (*((long*)&d)).GetHashCode();
		}

		public override string ToString() {
			return ToString(null, null);
		}

		public string ToString(IFormatProvider fp) {
			return ToString(null, fp);
		}

		public string ToString(string format) {
			return ToString(format, null);
		}

		public string ToString(string format, IFormatProvider fp) {
			NumberFormatInfo nfi = NumberFormatInfo.GetInstance(fp);
			return NumberFormatter.NumberToString(format, this.m_value, nfi);
		}

		#region IComparable Members

		public int CompareTo(object obj) {
			if (obj == null) {
				return 1;
			}
			if (!(obj is double)) {
				throw new ArgumentException();
			}
			return this.CompareTo((double)obj);
		}

		#endregion

		#region IComparable<double> Members

		public int CompareTo(double x) {
			if (double.IsNaN(this.m_value)) {
				return double.IsNaN(x) ? 0 : -1;
			}
			if (double.IsNaN(x)) {
				return 1;
			}
			return (this.m_value > x) ? 1 : ((this.m_value < x) ? -1 : 0);
		}

		#endregion

		#region IEquatable<double> Members

		public bool Equals(double x) {
			if (double.IsNaN(this.m_value)) {
				return double.IsNaN(x);
			}
			return this.m_value == x;
		}

		#endregion

	}
}

#endif
