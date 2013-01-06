using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRReturnInstruction : IRInstruction
    {
        public IRReturnInstruction() : base(IROpcode.Return) { }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
			if (ParentMethod.ReturnType != null) Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget));
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRReturnInstruction(), pNewMethod); }

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
			if (ParentMethod.ReturnType != null)
			{
				var sA = pLIRMethod.RequestLocal(Sources[0].GetTypeOfLocation());
				Sources[0].LoadTo(pLIRMethod, sA);
				new LIRInstructions.Return(pLIRMethod, sA, sA.Type);
				pLIRMethod.ReleaseLocal(sA);
			}
			else
			{
				new LIRInstructions.Return(pLIRMethod);
			}
		}

		public override string ToString()
		{
			if (ParentMethod.ReturnType != null)
				return "Return " + Sources[0];
			else
				return "Return";
		}
	}
}
