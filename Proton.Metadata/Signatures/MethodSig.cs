using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Signatures
{
    public sealed class MethodSig
    {
        public CLIFile CLIFile = null;

        public bool HasThis = false;
        public bool ExplicitThis = false;
        public bool Default = false;
        public bool CCall = false;
        public bool STDCall = false;
        public bool ThisCall = false;
        public bool FastCall = false;
        public bool VarArg = false;
        public bool Generic = false;
        public uint GenParamCount = 0;
        public SigRetType RetType = null;
        public List<SigParam> Params = null;
        public bool HasSentinel = false;
        public uint SentinelIndex = 0;

        public MethodSig(CLIFile pCLIFile, byte[] pSignature, ref int pCursor)
        {
            CLIFile = pCLIFile;

            byte callingConvention = pSignature[pCursor++];
            HasThis = (callingConvention & CallingConvention.HasThis) != 0;
            ExplicitThis = (callingConvention & CallingConvention.ExplicitThis) != 0;
            if ((callingConvention & CallingConvention.HasThis) != 0) callingConvention ^= CallingConvention.HasThis;
            if ((callingConvention & CallingConvention.ExplicitThis) != 0) callingConvention ^= CallingConvention.ExplicitThis;
            Default = callingConvention == CallingConvention.Default;
            CCall = callingConvention == CallingConvention.CCall;
            STDCall = callingConvention == CallingConvention.STDCall;
            ThisCall = callingConvention == CallingConvention.ThisCall;
            FastCall = callingConvention == CallingConvention.FastCall;
            VarArg = callingConvention == CallingConvention.VarArgs;
            Generic = callingConvention == CallingConvention.Generic;
            if (Generic) GenParamCount = CLIFile.ReadCompressedUnsigned(pSignature, ref pCursor);
            uint paramCount = CLIFile.ReadCompressedUnsigned(pSignature, ref pCursor);
            Params = new List<SigParam>((int)paramCount);
            RetType = new SigRetType(CLIFile, pSignature, ref pCursor);
            if (paramCount > 0)
            {
                for (uint paramIndex = 0; paramIndex < paramCount; ++paramIndex)
                {
                    if (pSignature[pCursor] == (byte)SigElementType.Sentinel)
                    {
                        HasSentinel = true;
                        SentinelIndex = paramIndex;
                        ++pCursor;
                    }
                    Params.Add(new SigParam(CLIFile, pSignature, ref pCursor));
                }
            }
        }
    }
}
