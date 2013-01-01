using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRArgListInstruction : IRInstruction
	{
		public IRArgListInstruction() : base(IROpcode.ArgList) { }

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			IRStackObject result = new IRStackObject();
			result.Type = ParentMethod.Assembly.AppDomain.System_RuntimeArgumentHandle;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
			result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, ParentMethod.Assembly.AppDomain.System_RuntimeArgumentHandle);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
			pStack.Push(result);
		}

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRArgListInstruction(), pNewMethod); }

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
			throw new NotSupportedException();
		}
	}
}
