using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRNopInstruction : IRInstruction
	{
		public bool ForceEmit { get; private set; }

		public IRNopInstruction(bool pForceEmit = false) : base(IROpcode.Nop) { ForceEmit = pForceEmit; }

		public override void Linearize(Stack<IRStackObject> pStack) { }

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRNopInstruction(ForceEmit), pNewMethod); }

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
			if (ForceEmit)
				new LIRInstructions.Nop(pLIRMethod);
		}

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("ForceEmit {0}", ForceEmit);
		}

		public override string ToString()
		{
			return "Nop " + ForceEmit;
		}
	}
}
