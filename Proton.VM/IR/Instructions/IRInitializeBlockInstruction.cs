using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRInitializeBlockInstruction : IRInstruction
    {
        public IRInitializeBlockInstruction() : base(IROpcode.InitializeBlock) { }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            IRStackObject size = pStack.Pop();
            IRStackObject srcAddr = pStack.Pop();
            IRStackObject destAddr = pStack.Pop();
            Sources.Add(new IRLinearizedLocation(destAddr.LinearizedTarget));
            Sources.Add(new IRLinearizedLocation(srcAddr.LinearizedTarget));
            Sources.Add(new IRLinearizedLocation(size.LinearizedTarget));
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRInitializeBlockInstruction(), pNewMethod); }
    }
}
