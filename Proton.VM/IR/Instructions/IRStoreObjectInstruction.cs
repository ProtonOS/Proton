using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRStoreObjectInstruction : IRInstruction
    {
        public IRType Type { get; private set; }

        public IRStoreObjectInstruction(IRType pType) : base(IROpcode.StoreObject) { Type = pType; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            Sources.Add(new IRLinearizedLocation(pStack.Pop().LinearizedTarget));

            Destination = new IRLinearizedLocation(IRLinearizedLocationType.Indirect);
            Destination.Indirect.Type = Type;
            Destination.Indirect.AddressLocation = new IRLinearizedLocation(pStack.Pop().LinearizedTarget);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRStoreObjectInstruction(Type), pNewMethod); }

        public override IRInstruction Transform() { return new IRMoveInstruction(this); }
    }
}
