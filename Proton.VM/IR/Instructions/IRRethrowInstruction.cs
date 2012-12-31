using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRRethrowInstruction : IRInstruction
	{
		public IRRethrowInstruction() : base(IROpcode.Rethrow) { }

		public override void Linearize(Stack<IRStackObject> pStack) { }

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRRethrowInstruction(), pNewMethod); }

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
		}
	}
}
