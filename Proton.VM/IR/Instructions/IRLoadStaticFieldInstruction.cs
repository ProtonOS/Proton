using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRLoadStaticFieldInstruction : IRInstruction
    {
        public IRField Field { get; private set; }

        public IRLoadStaticFieldInstruction(IRField pField) : base(IROpcode.LoadStaticField) { Field = pField; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            IRLinearizedLocation source = new IRLinearizedLocation(IRLinearizedLocationType.StaticField);
            source.StaticField.Field = Field;
            Sources.Add(source);

            IRStackObject result = new IRStackObject();
            result.Type = Field.Type;
            result.LinearizedTarget = new IRLinearizedLocation(IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, Field.Type);
            Destination = new IRLinearizedLocation(result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRLoadStaticFieldInstruction(Field), pNewMethod); }

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
