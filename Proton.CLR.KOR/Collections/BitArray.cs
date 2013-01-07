using System;
using System.Runtime.InteropServices;

namespace System.Collections
{
	public sealed class BitArray : ICollection, ICloneable
	{
		private int[] mArray;
		private int mLength;
		private int mVersion = 0;

		public BitArray(BitArray bits)
		{
			if (bits == null)
				throw new ArgumentNullException("bits");

			mLength = bits.mLength;
			mArray = new int[(mLength + 31) / 32];
			if (mArray.Length == 1)
				mArray[0] = bits.mArray[0];
			else
				Array.Copy(bits.mArray, mArray, mArray.Length);
		}

		public BitArray(bool[] values)
		{
			if (values == null)
				throw new ArgumentNullException("values");

			mLength = values.Length;
			mArray = new int[(mLength + 31) / 32];

			for (int i = 0; i < values.Length; i++)
				this[i] = values[i];
		}

		public BitArray(byte[] bytes)
		{
			if (bytes == null)
				throw new ArgumentNullException("bytes");

			mLength = bytes.Length * 8;
			mArray = new int[(mLength + 31) / 32];

			for (int i = 0; i < bytes.Length; i++)
				setByte(i, bytes[i]);
		}

		public BitArray(int[] values)
		{
			if (values == null)
				throw new ArgumentNullException("values");

			int arrlen = values.Length;
			mLength = arrlen * 32;
			mArray = new int[arrlen];
			Array.Copy(values, mArray, arrlen);
		}

		public BitArray(int length)
		{
			if (length < 0)
				throw new ArgumentOutOfRangeException("length");

			mLength = length;
			mArray = new int[(mLength + 31) / 32];
		}

		public BitArray(int length, bool defaultValue)
			: this(length)
		{
			if (defaultValue)
			{
				for (int i = 0; i < mArray.Length; i++)
					mArray[i] = ~0;
			}
		}

		byte getByte(int byteIndex)
		{
			int index = byteIndex / 4;
			int shift = (byteIndex % 4) * 8;

			int theByte = mArray[index] & (0xff << shift);

			return (byte)((theByte >> shift) & 0xff);
		}

		void setByte(int byteIndex, byte value)
		{
			int index = byteIndex / 4;
			int shift = (byteIndex % 4) * 8;

			mArray[index] &= ~(0xff << shift);
			mArray[index] |= value << shift;

			mVersion++;
		}

		void checkOperand(BitArray operand)
		{
			if (operand == null)
				throw new ArgumentNullException();
			if (operand.mLength != mLength)
				throw new ArgumentException();
		}

		public int Count
		{
			get { return mLength; }
		}

		public bool IsReadOnly
		{
			get { return false; }
		}

		public bool IsSynchronized
		{
			get { return false; }
		}

		public bool this[int index]
		{
			get { return Get(index); }
			set { Set(index, value); }
		}

		public int Length
		{
			get { return mLength; }
			set
			{
				if (mLength == value)
					return;

				if (value < 0)
					throw new ArgumentOutOfRangeException();

				if (value > mLength)
				{
					int numints = (value + 31) / 32;
					int old_numints = (mLength + 31) / 32;
					if (numints > mArray.Length)
					{
						int[] newArr = new int[numints];
						Array.Copy(mArray, newArr, mArray.Length);
						mArray = newArr;
					}
					else
					{
						Array.Clear(mArray, old_numints, numints - old_numints);
					}

					int mask = mLength % 32;
					if (mask > 0)
						mArray[old_numints - 1] &= (1 << mask) - 1;
				}

				mLength = value;
				mVersion++;
			}
		}

		public object SyncRoot
		{
			get { return this; }
		}

		public object Clone()
		{
			return new BitArray(this);
		}

		public void CopyTo(Array array, int index)
		{
			if (array == null)
				throw new ArgumentNullException("array");
			if (index < 0)
				throw new ArgumentOutOfRangeException("index");

			if (array.Rank != 1)
				throw new ArgumentException("array", "Array rank must be 1");

			if (index >= array.Length && mLength > 0)
				throw new ArgumentException("index", "index is greater than array.Length");

			if (array is bool[])
			{
				if (array.Length - index < mLength)
					throw new ArgumentException();

				bool[] barray = (bool[])array;

				for (int i = 0; i < mLength; i++)
					barray[index + i] = this[i];

			}
			else if (array is byte[])
			{
				int numbytes = (mLength + 7) / 8;

				if ((array.Length - index) < numbytes)
					throw new ArgumentException();

				byte[] barray = (byte[])array;
				for (int i = 0; i < numbytes; i++)
					barray[index + i] = getByte(i);

			}
			else if (array is int[])
			{

				Array.Copy(mArray, 0, array, index, (mLength + 31) / 32);

			}
			else
			{
				throw new ArgumentException("array", "Unsupported type");
			}
		}

		public BitArray Not()
		{
			int ints = (mLength + 31) / 32;
			for (int i = 0; i < ints; i++)
				mArray[i] = ~mArray[i];

			mVersion++;
			return this;
		}

		public BitArray And(BitArray value)
		{
			checkOperand(value);

			int ints = (mLength + 31) / 32;
			for (int i = 0; i < ints; i++)
				mArray[i] &= value.mArray[i];

			mVersion++;
			return this;
		}

		public BitArray Or(BitArray value)
		{
			checkOperand(value);

			int ints = (mLength + 31) / 32;
			for (int i = 0; i < ints; i++)
				mArray[i] |= value.mArray[i];

			mVersion++;
			return this;
		}

		public BitArray Xor(BitArray value)
		{
			checkOperand(value);

			int ints = (mLength + 31) / 32;
			for (int i = 0; i < ints; i++)
				mArray[i] ^= value.mArray[i];

			mVersion++;
			return this;
		}

		public bool Get(int index)
		{
			if (index < 0 || index >= mLength)
				throw new ArgumentOutOfRangeException();

			return (mArray[index >> 5] & (1 << (index & 31))) != 0;
		}

		public void Set(int index, bool value)
		{
			if (index < 0 || index >= mLength)
				throw new ArgumentOutOfRangeException();

			if (value)
				mArray[index >> 5] |= (1 << (index & 31));
			else
				mArray[index >> 5] &= ~(1 << (index & 31));

			mVersion++;
		}

		public void SetAll(bool value)
		{
			if (value)
			{
				for (int i = 0; i < mArray.Length; i++)
					mArray[i] = ~0;
			}
			else
				Array.Clear(mArray, 0, mArray.Length);

			mVersion++;
		}

		public IEnumerator GetEnumerator()
		{
			return new BitArrayEnumerator(this);
		}

		class BitArrayEnumerator : IEnumerator, ICloneable
		{
			BitArray mParent;
			bool mCurrent;
			int mIndex;
			int mVersion;

			public object Clone()
			{
				return MemberwiseClone();
			}

			public BitArrayEnumerator(BitArray ba)
			{
				mIndex = -1;
				mParent = ba;
				mVersion = ba.mVersion;
			}

			public object Current
			{
				get
				{
					if (mIndex == -1)
						throw new InvalidOperationException("Enum not started");
					if (mIndex >= mParent.Count)
						throw new InvalidOperationException("Enum Ended");

					return mCurrent;
				}
			}

			public bool MoveNext()
			{
				checkVersion();

				if (mIndex < (mParent.Count - 1))
				{
					mCurrent = mParent[++mIndex];
					return true;
				}

				mIndex = mParent.Count;
				return false;
			}

			public void Reset()
			{
				checkVersion();
				mIndex = -1;
			}

			void checkVersion()
			{
				if (mVersion != mParent.mVersion)
					throw new InvalidOperationException();
			}
		}
	}
}
