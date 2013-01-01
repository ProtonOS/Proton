using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRMoveInstruction : IRInstruction
    {
        public IRMoveInstruction() : base(IROpcode.Move) { }
        public IRMoveInstruction(IRInstruction pInstruction) : base(IROpcode.Move)
        {
            ILOffset = pInstruction.ILOffset;
            IRIndex = pInstruction.IRIndex;
            ParentMethod = pInstruction.ParentMethod;
            Sources.AddRange(pInstruction.Sources);
            Destination = pInstruction.Destination;
        }

        public override void Linearize(Stack<IRStackObject> pStack) { throw new InvalidOperationException(); }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRMoveInstruction(), pNewMethod); }

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
			var sA = pLIRMethod.RequestLocal(Sources[0].GetTypeOfLocation());
			Sources[0].LoadTo(pLIRMethod, sA);
			var dest = pLIRMethod.RequestLocal(Destination.GetTypeOfLocation());
			new LIRInstructions.Move(pLIRMethod, sA, dest, dest.Type);
			pLIRMethod.ReleaseLocal(sA);
			Destination.StoreTo(pLIRMethod, dest);
			pLIRMethod.ReleaseLocal(dest);
		}
	}
}
