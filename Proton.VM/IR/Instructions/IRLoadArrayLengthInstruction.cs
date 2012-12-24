using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRLoadArrayLengthInstruction : IRInstruction
    {
        public IRLoadArrayLengthInstruction() : base(IROpcode.LoadArrayLength) { }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
			IRLinearizedLocation source = new IRLinearizedLocation(this, IRLinearizedLocationType.ArrayLength);
			source.ArrayLength.ArrayLocation = new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget);
            Sources.Add(source);

            IRStackObject result = new IRStackObject();
            result.Type = ParentMethod.Assembly.AppDomain.System_Int32;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, ParentMethod.Assembly.AppDomain.System_Int32);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRLoadArrayLengthInstruction(), pNewMethod); }

        public override IRInstruction Transform() { return new IRMoveInstruction(this); }
    }
}
