namespace System
{
	public static class Buffer
	{
		public unsafe static int ByteLength(Array array)
		{
			if (array == null) throw new ArgumentNullException("array");
			Type.TypeData* typeData = array.GetType().GetTypeDataPointer();
			return (int)typeData->ArrayElementType->StackSize * array.Length;
		}

		public unsafe static byte GetByte(Array array, int index)
		{
			if (array == null) throw new ArgumentNullException("array");
			// TODO: This may need to be adjusted for MSB systems
			return *((byte*)array.Internal_ReferenceToPointer() + sizeof(int) + index);
		}

		public unsafe static void SetByte(Array array, int index, byte value)
		{
			if (array == null) throw new ArgumentNullException("array");
			// TODO: This may need to be adjusted for MSB systems
			*((byte*)array.Internal_ReferenceToPointer() + sizeof(int) + index) = value;
		}

		public unsafe static void BlockCopy(Array src, int srcOffset, Array dst, int dstOffset, int count)
		{
			if (src == null) throw new ArgumentNullException("src");
			if (dst == null) throw new ArgumentNullException("dst");
			if (srcOffset < 0 || dstOffset < 0 || count < 0) throw new ArgumentOutOfRangeException();
			if ((srcOffset + count) > ByteLength(src)) throw new ArgumentException("srcOffset");
			if ((dstOffset + count) > ByteLength(dst)) throw new ArgumentException("dstOffset");
			// TODO: This may need to be adjusted for MSB systems
			Internal_FastCopy((byte*)src.Internal_ReferenceToPointer() + sizeof(int) + srcOffset, (byte*)dst.Internal_ReferenceToPointer() + sizeof(int) + dstOffset, count);
		}
	}
}
