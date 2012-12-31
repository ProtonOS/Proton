using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRPopInstruction : IRInstruction
    {
        public IRPopInstruction() : base(IROpcode.Pop) { }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
			Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget));
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRPopInstruction(), pNewMethod); }

		public override void ConvertToLIR(LIRMethod pLIRMethod) { }
	}
}
