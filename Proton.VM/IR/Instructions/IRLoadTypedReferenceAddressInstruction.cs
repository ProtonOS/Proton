using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRLoadTypedReferenceAddressInstruction : IRInstruction
	{
		private IRType mType = null;
		public IRType Type { get { return mType; } private set { mType = value; } }

		public IRLoadTypedReferenceAddressInstruction(IRType pType) : base(IROpcode.LoadTypedReferenceAddress) { Type = pType; }

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget));

			IRType pointerType = ParentMethod.Assembly.AppDomain.GetManagedPointerType(Type);
			IRStackObject result = new IRStackObject();
			result.Type = pointerType;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
			result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, pointerType);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
			pStack.Push(result);
		}

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRLoadTypedReferenceAddressInstruction(Type), pNewMethod); }

		public override bool Resolved { get { return Type.Resolved; } }
		public override void Resolve()
		{
			base.Resolve();
			Type.Resolve(ref mType, ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters);
		}

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
			var sTypedReference = pLIRMethod.RequestLocal(Sources[0].GetTypeOfLocation());
			Sources[0].LoadTo(pLIRMethod, sTypedReference);
			var sAddressPointer = pLIRMethod.RequestLocal(Destination.GetTypeOfLocation());
			new LIRInstructions.Math(pLIRMethod, sTypedReference, (LIRImm)VMConfig.PointerSizeForTarget, sAddressPointer, LIRInstructions.MathOperation.Add, sAddressPointer.Type);
			Destination.StoreTo(pLIRMethod, sAddressPointer);
			pLIRMethod.ReleaseLocal(sAddressPointer);
			pLIRMethod.ReleaseLocal(sTypedReference);
		}

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Type {0}", Type.ToString());
		}
	}
}
