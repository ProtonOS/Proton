namespace System.Text
{
	public class UnicodeEncoding : Encoding
	{
		public override string GetString(byte[] bytes, int index, int count)
		{
			// Not accurate, but it'll work for now
			int len = count >> 1;
			char[] buf = new char[len];
			int temp;
			for (int i = 0; i < len; ++i)
			{
				temp = bytes[index + (i << 1)];
				temp |= bytes[index + (i << 1) + 1] << 8;
				buf[i] = (char)temp;
			}
			return new string(buf);
		}
	}
}
