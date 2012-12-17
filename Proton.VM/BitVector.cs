using System;

namespace System.Collections.Generic
{
	public sealed class BitVector : IEquatable<BitVector>
	{
		private readonly int mBitCount;
		private readonly int[] mData;

		public BitVector(BitVector pBitVector)
		{
			mBitCount = pBitVector.mBitCount;
			int dataLength = pBitVector.mData.Length;
			mData = new int[dataLength];
			Array.Copy(pBitVector.mData, mData, dataLength);
		}

		public BitVector(bool[] pBits)
		{
			mBitCount = pBits.Length;
			int dataLength = mBitCount >> 5;
			int bitsOverflowed = mBitCount & 0x1F;
			if (bitsOverflowed != 0) ++dataLength;
			mData = new int[dataLength];
			for (int index = 0; index < mBitCount; ++index)
			{
				if (pBits[index]) mData[(index >> 5)] |= (1 << (index & 0x1F));
			}
		}

		public BitVector(int pBits, bool pInitialValue)
		{
			mBitCount = pBits;
			int dataLength = mBitCount >> 5;
			int dataFullBlocks = dataLength;
			int bitsOverflowed = mBitCount & 0x1F;
			if (bitsOverflowed != 0) ++dataLength;
			mData = new int[dataLength];
			if (pInitialValue)
			{
				for (int index = 0; index < dataFullBlocks; ++index) mData[index] = -1;
				if (bitsOverflowed != 0) mData[dataLength - 1] = (1 << bitsOverflowed) - 1;
			}
		}

		public BitVector(int pBits) : this(pBits, false) { }

		public int Count { get { return mBitCount; } }

		public bool Get(int pIndex)
		{
			if (pIndex < 0 || pIndex >= mBitCount) throw new ArgumentOutOfRangeException("pIndex");
			return (mData[(pIndex >> 5)] & (1 << (pIndex & 0x1F))) != 0;
		}

		public void Set(int pIndex, bool pValue)
		{
			if (pIndex < 0 || pIndex >= mBitCount) throw new ArgumentOutOfRangeException("pIndex");
			if (pValue) mData[(pIndex >> 5)] |= (1 << (pIndex & 0x1F));
			else mData[(pIndex >> 5)] &= ~(1 << (pIndex & 0x1F));
		}

		public void SetAll(bool pValue)
		{
			int dataLength = mData.Length;
			if (pValue)
			{
				int dataFullBlocks = dataLength;
				int bitsOverflowed = mBitCount & 0x1F;
				if (bitsOverflowed != 0) --dataFullBlocks;
				for (int index = 0; index < dataFullBlocks; ++index) mData[index] = -1;
				if (bitsOverflowed != 0) mData[dataLength - 1] = (1 << bitsOverflowed) - 1;
			}
			else Array.Clear(mData, 0, mData.Length);
		}

		public BitVector AndEquals(BitVector pBitVector)
		{
			if (mBitCount != pBitVector.mBitCount) throw new ArgumentException("different number of bits");
			int dataLength = mData.Length;
			for (int index = 0; index < dataLength; ++index) mData[index] &= pBitVector.mData[index];
			return this;
		}

		public BitVector And(BitVector pBitVector)
		{
			if (mBitCount != pBitVector.mBitCount) throw new ArgumentException("different number of bits");
			BitVector bitVector = new BitVector(this);
			return bitVector.AndEquals(pBitVector);
		}

		public BitVector OrEquals(BitVector pBitVector)
		{
			if (mBitCount != pBitVector.mBitCount) throw new ArgumentException("different number of bits");
			int dataLength = mData.Length;
			for (int index = 0; index < dataLength; ++index) mData[index] |= pBitVector.mData[index];
			return this;
		}

		public BitVector Or(BitVector pBitVector)
		{
			if (mBitCount != pBitVector.mBitCount) throw new ArgumentException("different number of bits");
			BitVector bitVector = new BitVector(this);
			return bitVector.OrEquals(pBitVector);
		}

		public BitVector XorEquals(BitVector pBitVector)
		{
			if (mBitCount != pBitVector.mBitCount) throw new ArgumentException("different number of bits");
			int dataLength = mData.Length;
			for (int index = 0; index < dataLength; ++index) mData[index] ^= pBitVector.mData[index];
			return this;
		}

		public BitVector Xor(BitVector pBitVector)
		{
			if (mBitCount != pBitVector.mBitCount) throw new ArgumentException("different number of bits");
			BitVector bitVector = new BitVector(this);
			return bitVector.XorEquals(pBitVector);
		}

		public bool Equals(BitVector pBitVector)
		{
			bool equal = mBitCount == pBitVector.mBitCount;
			if (!equal) return false;
			int dataLength = mData.Length;
			for (int index = 0; index < dataLength && (equal = (mData[index] == pBitVector.mData[index])); ++index) ;
			return equal;
		}
	}
}
