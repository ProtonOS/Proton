using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRLoadObjectInstruction : IRInstruction
	{
		private IRType mType = null;
		public IRType Type { get { return mType; } private set { mType = value; } }

		public IRLoadObjectInstruction(IRType pType) : base(IROpcode.LoadObject) { Type = pType; }

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			IRLinearizedLocation source = new IRLinearizedLocation(this, IRLinearizedLocationType.Indirect);
			source.Indirect.Type = Type;
			source.Indirect.AddressLocation = new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget);
			Sources.Add(source);

			IRStackObject result = new IRStackObject();
			result.Type = Type;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
			result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, Type);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
			pStack.Push(result);
		}

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRLoadObjectInstruction(Type), pNewMethod); }

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
