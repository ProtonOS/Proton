using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Proton.Metadata.Signatures
{
    public sealed class MethodSpecSig
    {
        public CLIFile CLIFile = null;

        public List<SigType> GenArgs = null;

        public MethodSpecSig(CLIFile pCLIFile, byte[] pSignature, ref int pCursor)
        {
            CLIFile = pCLIFile;

            ++pCursor;
            uint genArgsCount = CLIFile.ReadCompressedUnsigned(pSignature, ref pCursor);
            GenArgs = new List<SigType>((int)genArgsCount);
            for (uint genArgsIndex = 0; genArgsIndex < genArgsCount; ++genArgsIndex) GenArgs.Add(new SigType(CLIFile, pSignature, ref pCursor));
        }
    }
}
