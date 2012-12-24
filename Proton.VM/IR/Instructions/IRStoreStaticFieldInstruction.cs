using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRStoreStaticFieldInstruction : IRInstruction
    {
        public IRField Field = null;

        public IRStoreStaticFieldInstruction(IRField pField) : base(IROpcode.StoreStaticField) { Field = pField; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
			Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget));

			Destination = new IRLinearizedLocation(this, IRLinearizedLocationType.StaticField);
            Destination.StaticField.Field = Field;
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRStoreStaticFieldInstruction(Field), pNewMethod); }

        public override IRInstruction Transform() { return new IRMoveInstruction(this); }

		public override bool Resolved { get { return Field.Resolved; } }
		public override void Resolve()
		{
			base.Resolve();
			Field.Resolve(ref Field, ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters);
		}

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Field {0}", Field.ToString());
		}
	}
}
