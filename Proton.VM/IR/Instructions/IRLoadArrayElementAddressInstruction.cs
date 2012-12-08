using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRLoadArrayElementAddressInstruction : IRInstruction
    {
        public IRType Type { get; private set; }

        public IRLoadArrayElementAddressInstruction(IRType pType) : base(IROpcode.LoadArrayElementAddress) { Type = pType; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            IRLinearizedLocation source = new IRLinearizedLocation(IRLinearizedLocationType.ArrayElementAddress);
            source.ArrayElementAddress.IndexLocation = new IRLinearizedLocation(pStack.Pop().LinearizedTarget);
            source.ArrayElementAddress.ArrayLocation = new IRLinearizedLocation(pStack.Pop().LinearizedTarget);
            source.ArrayElementAddress.ElementType = Type;
            Sources.Add(source);

            IRStackObject result = new IRStackObject();
            result.Type = ParentMethod.Assembly.AppDomain.System_IntPtr;
            result.LinearizedTarget = new IRLinearizedLocation(IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, ParentMethod.Assembly.AppDomain.System_IntPtr);
            Destination = new IRLinearizedLocation(result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRLoadArrayElementAddressInstruction(Type), pNewMethod); }

        public override IRInstruction Transform() { return new IRMoveInstruction(this); }
    }
}
