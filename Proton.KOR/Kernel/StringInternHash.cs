using System;

namespace System.Kernel
{
	internal unsafe static class StringInternHash
	{
		private static uint Compute(byte* data, uint length)
		{
			uint nblocks = length >> 2;
			uint hash = 0;
			uint* blocks = (uint*)(data + (nblocks << 2));
			uint block = 0;
			for (int index = -((int)nblocks); index != 0; ++index)
			{
				block = blocks[index];
				block *= 0xcc9e2d51;
				block = uint.RotateLeft(block, 15);
				block *= 0x1b873593;

				hash ^= block;
				hash = uint.RotateLeft(hash, 13);
				hash = hash * 5 + 0xe6546b64;
			}

			byte* tail = data + (nblocks << 2);
			block = 0;
			uint remainder = length & 0x03;
			if (length == 3) block ^= (uint)tail[2] << 16;
			if (length >= 2) block ^= (uint)tail[1] << 8;
			if (length >= 1)
			{
				block ^= (uint)tail[0];
				block *= 0xcc9e2d51;
				block = uint.RotateLeft(block, 15);
				block *= 0x1b873593;
				hash ^= block;
			}

			hash ^= length;
			hash ^= hash >> 16;
			hash *= 0x85ebca6b;
			hash ^= hash >> 13;
			hash *= 0xc2b2ae35;
			hash ^= hash >> 16;
			return hash;
		}
	}
}
