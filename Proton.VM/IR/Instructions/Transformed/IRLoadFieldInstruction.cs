using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRLoadFieldInstruction : IRInstruction
	{
		private IRField mField = null;
		public IRField Field { get { return mField; } private set { mField = value; } }

		public IRLoadFieldInstruction(IRField pField) : base(IROpcode.LoadField) { Field = pField; }

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			IRLinearizedLocation source = new IRLinearizedLocation(this, IRLinearizedLocationType.Field);
			source.Field.Field = Field;
			source.Field.FieldLocation = new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget);
			Sources.Add(source);

			IRStackObject result = new IRStackObject();
			result.Type = Field.Type;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
			result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, Field.Type);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
			pStack.Push(result);
		}

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRLoadFieldInstruction(Field), pNewMethod); }

		public override IRInstruction Transform() { return new IRMoveInstruction(this); }

		public override bool Resolved { get { return Field.Resolved; } }
		public override void Resolve()
		{
			base.Resolve();
			Field.Resolve(ref mField, ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters);
		}

		public override void ConvertToLIR(LIRMethod pLIRMethod) { }

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Field {0}", Field.ToString());
		}
	}
}
