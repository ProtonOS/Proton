using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRNegateInstruction : IRInstruction
    {
        public IRNegateInstruction() : base(IROpcode.Negate) { }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            IRStackObject value = pStack.Pop();
			Sources.Add(new IRLinearizedLocation(this, value.LinearizedTarget));

            IRStackObject result = new IRStackObject();
            result.Type = value.Type;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, value.Type);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRNegateInstruction(), pNewMethod); }
    }
}
