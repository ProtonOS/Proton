using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRCopyBlockInstruction : IRInstruction
    {
        public IRCopyBlockInstruction() : base(IROpcode.CopyBlock) { }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            IRStackObject size = pStack.Pop();
            IRStackObject srcAddr = pStack.Pop();
            IRStackObject destAddr = pStack.Pop();
			Sources.Add(new IRLinearizedLocation(this, destAddr.LinearizedTarget));
			Sources.Add(new IRLinearizedLocation(this, srcAddr.LinearizedTarget));
			Sources.Add(new IRLinearizedLocation(this, size.LinearizedTarget));
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRCopyBlockInstruction(), pNewMethod); }
    }
}
