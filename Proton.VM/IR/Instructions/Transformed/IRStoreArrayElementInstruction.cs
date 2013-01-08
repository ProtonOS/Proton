using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRStoreArrayElementInstruction : IRInstruction
	{
		private IRType mType = null;
		public IRType Type { get { return mType; } private set { mType = value; } }

		public IRStoreArrayElementInstruction(IRType pType) : base(IROpcode.StoreArrayElement) { Type = pType; }

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget));

			Destination = new IRLinearizedLocation(this, IRLinearizedLocationType.ArrayElement);
			Destination.ArrayElement.IndexLocation = new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget);
			var arraySource = pStack.Pop();
			Destination.ArrayElement.ArrayLocation = new IRLinearizedLocation(this, arraySource.LinearizedTarget);
			if (Type == null)
			{
				Type = arraySource.Type.ArrayElementType;
			}
			if (Type == null) throw new Exception();
			Destination.ArrayElement.ElementType = Type;
		}

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRStoreArrayElementInstruction(Type), pNewMethod); }

		public override IRInstruction Transform() { return new IRMoveInstruction(this); }

		public override bool Resolved { get { return Type.Resolved; } }
		public override void Resolve()
		{
			base.Resolve();
			Type.Resolve(ref mType, ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters);
		}

		public override void ConvertToLIR(LIRMethod pLIRMethod) { }

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Type {0}", Type.ToString());
		}
	}
}
