using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRMoveInstruction : IRInstruction
    {
        public IRMoveInstruction() : base(IROpcode.Move) { }
        public IRMoveInstruction(IRInstruction pInstruction) : base(IROpcode.Move)
        {
            ILOffset = pInstruction.ILOffset;
            IRIndex = pInstruction.IRIndex;
            ParentMethod = pInstruction.ParentMethod;
            Sources.AddRange(pInstruction.Sources);
            Destination = pInstruction.Destination;
        }

        public override void Linearize(Stack<IRStackObject> pStack) { throw new InvalidOperationException(); }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRMoveInstruction(), pNewMethod); }
    }
}
