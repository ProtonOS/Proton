using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRStoreFieldInstruction : IRInstruction
    {
        public IRField Field { get; private set; }

        public IRStoreFieldInstruction(IRField pField) : base(IROpcode.StoreField) { Field = pField; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            Sources.Add(new IRLinearizedLocation(pStack.Pop().LinearizedTarget));

            Destination = new IRLinearizedLocation(IRLinearizedLocationType.Field);
            Destination.Field.Field = Field;
            Destination.Field.FieldLocation = new IRLinearizedLocation(pStack.Pop().LinearizedTarget);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRStoreFieldInstruction(Field), pNewMethod); }

        public override IRInstruction Transform() { return new IRMoveInstruction(this); }

		public override bool Resolved { get { return Field.Resolved; } }
		public override void Resolve()
		{
			base.Resolve();
			Field.Resolve();
		}

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Field {0}", Field.ToString());
		}
	}
}
