using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRCheckFiniteInstruction : IRInstruction
	{
		public IRCheckFiniteInstruction() : base(IROpcode.CheckFinite) { }

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget));

			IRStackObject result = new IRStackObject();
			result.Type = ParentMethod.Assembly.AppDomain.System_Double;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
			result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, ParentMethod.Assembly.AppDomain.System_Double);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
			pStack.Push(result);
		}

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRCheckFiniteInstruction(), pNewMethod); }

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
		}
	}
}
