using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRNopInstruction : IRInstruction
    {
        public bool ForceEmit { get; private set; }

        public IRNopInstruction(bool pForceEmit) : base(IROpcode.Nop) { ForceEmit = pForceEmit; }

        public override void Linearize(Stack<IRStackObject> pStack) { }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRNopInstruction(ForceEmit), pNewMethod); }

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("ForceEmit {0}", ForceEmit);
		}
	}
}
