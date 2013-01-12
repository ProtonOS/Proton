namespace System.Text
{
	public class ASCIIEncoding : Encoding
	{
		public override byte[] GetBytes(string str)
		{
#warning Implement Me!
			return new byte[0];
		}
		public override string GetString(byte[] bytes, int index, int count)
		{
			char[] buf = new char[count];
			for (int i = 0; i < count; ++i) buf[i] = (char)bytes[index + i];
			return new string(buf);
		}
	}
}
