using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Proton.Metadata.Signatures
{
    public sealed class FieldSig
    {
        public CLIFile CLIFile = null;

        public List<SigCustomMod> Mods = new List<SigCustomMod>();
        public SigType Type = null;

        public FieldSig(CLIFile pCLIFile, byte[] pSignature, ref int pCursor)
        {
            CLIFile = pCLIFile;
            ++pCursor;
            while (pSignature[pCursor] == (byte)SigElementType.CustomModifier_Required ||
                   pSignature[pCursor] == (byte)SigElementType.CustomModifier_Optional)
            {
                Mods.Add(new SigCustomMod(CLIFile, pSignature, ref pCursor));
            }
            Type = new SigType(CLIFile, pSignature, ref pCursor);
        }
    }
}
