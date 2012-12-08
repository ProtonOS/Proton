using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRBreakInstruction : IRInstruction
    {
        public IRBreakInstruction() : base(IROpcode.Break) { }

        public override void Linearize(Stack<IRStackObject> pStack) { }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRBreakInstruction(), pNewMethod); }
    }
}
