using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRNewObjectInstruction : IRInstruction
    {
        public IRMethod Constructor { get; private set; }

        public IRNewObjectInstruction(IRMethod pConstructor) : base(IROpcode.NewObject) { Constructor = pConstructor; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            // Skip first parameter source, it is the 'this' reference that is put on the stack by newobj before constructor call
            for (int count = 1; count < Constructor.Parameters.Count; ++count) Sources.Add(new IRLinearizedLocation(pStack.Pop().LinearizedTarget));

            IRStackObject result = new IRStackObject();
            result.Type = Constructor.ParentType;
            result.LinearizedTarget = new IRLinearizedLocation(IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, Constructor.ParentType);
            Destination = new IRLinearizedLocation(result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRNewObjectInstruction(Constructor), pNewMethod); }
    }
}
