namespace System
{
    public static class BitConverter
    {
        public static readonly bool IsLittleEndian = AmILittleEndian();
		private static readonly bool SwappedWordsInDouble = DoubleWordsAreSwapped();

        private static unsafe bool AmILittleEndian()
        {
            int i = 1;
            byte b = *((byte*)&i);
            return (b == 1);
        }

		private static unsafe bool DoubleWordsAreSwapped()
		{
			double d = 1.0;
			byte* b = (byte*)&d;
			return b[2] == 0xf0;
		}

        public static unsafe long DoubleToInt64Bits(double value) { return *((long*)&value); }

		private unsafe static void PutBytes(byte* dst, byte[] src, int start_index, int count)
		{
			if (src == null)
				throw new ArgumentNullException("value");

			if (start_index < 0 || (start_index > src.Length - 1))
				throw new ArgumentOutOfRangeException("startIndex", "Index was"
					+ " out of range. Must be non-negative and less than the"
					+ " size of the collection.");

			// avoid integer overflow (with large pos/neg start_index values)
			if (src.Length - count < start_index)
				throw new ArgumentException("Destination array is not long"
					+ " enough to copy all the items in the collection."
					+ " Check array index and length.");

			for (int i = 0; i < count; i++)
				dst[i] = src[i + start_index];
		}


		public unsafe static float ToSingle(byte[] value, int startIndex)
		{
			float ret;
			PutBytes((byte*)&ret, value, startIndex, 4);
			return ret;
		}

		public unsafe static double ToDouble(byte[] value, int startIndex)
		{
			double ret;

			if (SwappedWordsInDouble)
			{
				byte* p = (byte*)&ret;
				if (value == null)
					throw new ArgumentNullException("value");

				if (startIndex < 0 || (startIndex > value.Length - 1))
					throw new ArgumentOutOfRangeException("startIndex", "Index was"
						+ " out of range. Must be non-negative and less than the"
						+ " size of the collection.");

				// avoid integer overflow (with large pos/neg start_index values)
				if (value.Length - 8 < startIndex)
					throw new ArgumentException("Destination array is not long"
						+ " enough to copy all the items in the collection."
						+ " Check array index and length.");

				p[0] = value[startIndex + 4];
				p[1] = value[startIndex + 5];
				p[2] = value[startIndex + 6];
				p[3] = value[startIndex + 7];
				p[4] = value[startIndex + 0];
				p[5] = value[startIndex + 1];
				p[6] = value[startIndex + 2];
				p[7] = value[startIndex + 3];

				return ret;
			}

			PutBytes((byte*)&ret, value, startIndex, 8);

			return ret;
		}
	}
}
