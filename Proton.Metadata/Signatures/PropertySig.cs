using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Proton.Metadata.Signatures
{
    public sealed class PropertySig
    {
        public CLIFile CLIFile = null;

        public bool HasThis = false;
        public List<SigCustomMod> Mods = new List<SigCustomMod>();
        public SigType Type = null;
        public List<SigParam> Params = null;

        public PropertySig(CLIFile pCLIFile, byte[] pSignature, ref int pCursor)
        {
            CLIFile = pCLIFile;

            HasThis = (pSignature[pCursor++] & CallingConvention.HasThis) != 0;
            uint paramCount = CLIFile.ReadCompressedUnsigned(pSignature, ref pCursor);
            while (pSignature[pCursor] == (byte)SigElementType.CustomModifier_Required ||
                   pSignature[pCursor] == (byte)SigElementType.CustomModifier_Optional)
            {
                Mods.Add(new SigCustomMod(CLIFile, pSignature, ref pCursor));
            }
            Type = new SigType(CLIFile, pSignature, ref pCursor);
            Params = new List<SigParam>((int)paramCount);
            for (uint paramIndex = 0; paramIndex < paramCount; ++paramIndex) Params.Add(new SigParam(CLIFile, pSignature, ref pCursor));
        }
    }
}
