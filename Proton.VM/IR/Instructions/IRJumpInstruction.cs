using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRJumpInstruction : IRInstruction
    {
        public IRMethod Target { get; private set; }

        public IRJumpInstruction(IRMethod pTarget) : base(IROpcode.Jump) { Target = pTarget; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            if (pStack.Count > 0) throw new OverflowException();
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRJumpInstruction(Target), pNewMethod); }
    }
}
