namespace System.Text
{
	public abstract class Encoding : ICloneable
	{
		private static Encoding ASCIIEncoding = null;
		private static Encoding UnicodeEncoding = null;

		public static Encoding ASCII { get { return ASCIIEncoding; } }
		public static Encoding Unicode { get { return UnicodeEncoding; } }

		public abstract string GetString(byte[] bytes, int index, int count);

		public object Clone() { return (Encoding)object.MemberwiseClone(this); }
	}
}
