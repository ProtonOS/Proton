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
    }
}
