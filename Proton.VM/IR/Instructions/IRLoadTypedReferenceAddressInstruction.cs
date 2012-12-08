using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRLoadTypedReferenceAddressInstruction : IRInstruction
    {
        public IRType Type { get; private set; }

        public IRLoadTypedReferenceAddressInstruction(IRType pType) : base(IROpcode.LoadTypedReferenceAddress) { Type = pType; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            Sources.Add(new IRLinearizedLocation(pStack.Pop().LinearizedTarget));

            IRType pointerType = ParentMethod.Assembly.AppDomain.GetPointerType(Type);
            IRStackObject result = new IRStackObject();
            result.Type = pointerType;
            result.LinearizedTarget = new IRLinearizedLocation(IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, pointerType);
            Destination = new IRLinearizedLocation(result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRLoadTypedReferenceAddressInstruction(Type), pNewMethod); }
    }
}
