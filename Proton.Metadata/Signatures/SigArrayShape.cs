using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Signatures
{
	public sealed class SigArrayShape
	{
		public CLIFile CLIFile = null;

		public uint Rank = 0;
		public List<uint> Sizes = null;
		public List<int> LowBounds = null;

		public SigArrayShape(CLIFile pCLIFile, byte[] pSignature, ref int pCursor)
		{
			CLIFile = pCLIFile;

			Rank = CLIFile.ReadCompressedUnsigned(pSignature, ref pCursor);
			uint sizeCount = CLIFile.ReadCompressedUnsigned(pSignature, ref pCursor);
			Sizes = new List<uint>((int)sizeCount);
			for (uint sizeIndex = 0; sizeIndex < sizeCount; ++sizeIndex) Sizes.Add(CLIFile.ReadCompressedUnsigned(pSignature, ref pCursor));
			uint lowBoundCount = CLIFile.ReadCompressedUnsigned(pSignature, ref pCursor);
			LowBounds = new List<int>((int)lowBoundCount);
			for (uint lowBoundIndex = 0; lowBoundIndex < lowBoundCount; ++lowBoundIndex) LowBounds.Add(CLIFile.ReadCompressedSigned(pSignature, ref pCursor));
		}
	}
}
