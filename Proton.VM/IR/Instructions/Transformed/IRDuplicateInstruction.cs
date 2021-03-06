using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRDuplicateInstruction : IRInstruction
	{
		public IRDuplicateInstruction() : base(IROpcode.Duplicate) { }

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			IRStackObject value = pStack.Peek();
			Sources.Add(new IRLinearizedLocation(this, value.LinearizedTarget));

			IRStackObject result = new IRStackObject();
			result.Type = value.Type;
			result.BoxedType = value.BoxedType;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
			result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, value.Type);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
			pStack.Push(result);
		}

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRDuplicateInstruction(), pNewMethod); }

		public override IRInstruction Transform() { return new IRMoveInstruction(this); }

		public override void ConvertToLIR(LIRMethod pLIRMethod) { }
	}
}
