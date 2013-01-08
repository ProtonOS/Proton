using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Signatures
{
	public sealed class SigParam
	{
		public CLIFile CLIFile = null;

		public List<SigCustomMod> Mods = null;
		public bool ByRef = false;
		public SigType Type = null;
		public bool TypedByRef = false;

		public SigParam(CLIFile pCLIFile, byte[] pSignature, ref int pCursor)
		{
			CLIFile = pCLIFile;

			while (pSignature[pCursor] == (byte)SigElementType.CustomModifier_Required ||
				   pSignature[pCursor] == (byte)SigElementType.CustomModifier_Optional)
			{
				Mods.Add(new SigCustomMod(CLIFile, pSignature, ref pCursor));
			}
			if (pSignature[pCursor] == (byte)SigElementType.TypedByReference)
			{
				TypedByRef = true;
				++pCursor;
				return;
			}
			if (pSignature[pCursor] == (byte)SigElementType.ByReference)
			{
				ByRef = true;
				++pCursor;
			}
			Type = new SigType(CLIFile, pSignature, ref pCursor);
		}
	}
}
