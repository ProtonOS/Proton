using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Signatures
{
	public sealed class SigCustomMod
	{
		public CLIFile CLIFile = null;

		public bool Optional = false;
		public uint TypeDefOrRefOrSpecToken = 0;

		public SigCustomMod(CLIFile pCLIFile, byte[] pSignature, ref int pCursor)
		{
			CLIFile = pCLIFile;

			Optional = pSignature[pCursor++] == (byte)SigElementType.CustomModifier_Optional;
			TypeDefOrRefOrSpecToken = CLIFile.ReadCompressedUnsigned(pSignature, ref pCursor);
		}
	}
}
