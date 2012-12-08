using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRCopyObjectInstruction : IRInstruction
    {
        public IRType Type { get; private set; }

        public IRCopyObjectInstruction(IRType pType) : base(IROpcode.CopyObject) { Type = pType; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            IRStackObject srcAddr = pStack.Pop();
            IRStackObject destAddr = pStack.Pop();
            Sources.Add(new IRLinearizedLocation(destAddr.LinearizedTarget));
            Sources.Add(new IRLinearizedLocation(srcAddr.LinearizedTarget));
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRCopyObjectInstruction(Type), pNewMethod); }
    }
}
