using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRAndInstruction : IRInstruction
    {
        public IRAndInstruction() : base(IROpcode.And) { }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            IRStackObject value2 = pStack.Pop();
            IRStackObject value1 = pStack.Pop();

			Sources.Add(new IRLinearizedLocation(this, value1.LinearizedTarget));
			Sources.Add(new IRLinearizedLocation(this, value2.LinearizedTarget));

            IRType resultType = ParentMethod.Assembly.AppDomain.BinaryNumericResult(value1.Type, value2.Type);
            IRStackObject result = new IRStackObject();
            result.Type = resultType;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, resultType);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRAndInstruction(), pNewMethod); }
    }
}
