using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRLoadTypedReferenceTypeInstruction : IRInstruction
	{
		public IRLoadTypedReferenceTypeInstruction() : base(IROpcode.LoadTypedReferenceType) { }

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget));

			IRStackObject result = new IRStackObject();
			result.Type = ParentMethod.Assembly.AppDomain.System_RuntimeTypeHandle;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
			result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, ParentMethod.Assembly.AppDomain.System_RuntimeTypeHandle);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
			pStack.Push(result);
		}

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRLoadTypedReferenceTypeInstruction(), pNewMethod); }

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
			var sTypedReference = pLIRMethod.RequestLocal(Sources[0].GetTypeOfLocation().GetManagedPointerType());
			Sources[0].LoadAddressTo(pLIRMethod, sTypedReference);
			new LIRInstructions.Move(pLIRMethod, new Indirect(sTypedReference), sTypedReference, sTypedReference.Type);
			Destination.StoreTo(pLIRMethod, sTypedReference);
			pLIRMethod.ReleaseLocal(sTypedReference);
		}
	}
}
