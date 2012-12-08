using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRNewArrayInstruction : IRInstruction
    {
        public IRType Type { get; private set; }

        public IRNewArrayInstruction(IRType pType) : base(IROpcode.NewArray) { Type = pType; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            Sources.Add(new IRLinearizedLocation(pStack.Pop().LinearizedTarget));

            IRType arrayType = ParentMethod.Assembly.AppDomain.GetArrayType(Type);
            IRStackObject result = new IRStackObject();
            result.Type = arrayType;
            result.LinearizedTarget = new IRLinearizedLocation(IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, arrayType);
            Destination = new IRLinearizedLocation(result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRNewArrayInstruction(Type), pNewMethod); }
    }
}
