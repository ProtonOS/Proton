using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRThrowInstruction : IRInstruction
    {
        public IRThrowInstruction() : base(IROpcode.Throw) { }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
			Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget));
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRThrowInstruction(), pNewMethod); }

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
		}

		public override string ToString()
		{
			return "Throw " + Sources[0];
		}
	}
}
