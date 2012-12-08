using Proton.Metadata.Tables;
using System;
using System.Collections.Generic;

namespace Proton.Metadata.Signatures
{
    public sealed class SigArg
    {
        public SigArgType ArgType = SigArgType.Param;
        public string Name = null;
        public SigElementType PrimaryType = SigElementType.End;
        public SigElementType SecondaryType = SigElementType.End;
        public TypeDefData EnumType = null;
        public SigElementType EnumBaseType = SigElementType.End;
        public List<SigElem> Elems = null;
    }
}
