using Proton.Metadata.Tables;
using Proton.Metadata.Signatures;
using Proton.VM.IL;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR
{
    public sealed class IRInterfaceImplementation
    {
        public IRAssembly Assembly = null;
        public IRType ParentType = null;
        public IRType InterfaceType = null;

        public IRInterfaceImplementation(IRAssembly pAssembly)
        {
            Assembly = pAssembly;
        }
    }
}
