using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRNotInstruction : IRInstruction
    {
        public IRNotInstruction() : base(IROpcode.Not) { }

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

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRNotInstruction(), pNewMethod); }

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
		}
	}
}
