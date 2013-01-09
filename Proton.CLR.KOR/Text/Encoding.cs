namespace System.Text
{
	public abstract class Encoding : ICloneable
	{
		private static ASCIIEncoding ASCIIEncoding = new ASCIIEncoding();
		private static UnicodeEncoding UnicodeEncoding = new UnicodeEncoding();

		public static Encoding ASCII { get { return ASCIIEncoding; } }
		public static Encoding Unicode { get { return UnicodeEncoding; } }

		public abstract string GetString(byte[] bytes, int index, int count);

		public object Clone() { return (Encoding)object.MemberwiseClone(this); }
	}
}
