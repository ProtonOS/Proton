using Proton.Metadata.Tables;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRUnboxInstruction : IRInstruction
    {
        public IRType Type = null;
        public bool Value = false;

        public IRUnboxInstruction(IRType pType, bool pValue) : base(IROpcode.Unbox)
        {
            Type = pType;
            Value = pValue;
        }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            Sources.Add(new IRLinearizedLocation(pStack.Pop().LinearizedTarget));

            IRType resultType = Type;
            if (!Value) resultType = ParentMethod.Assembly.AppDomain.GetPointerType(Type);
            IRStackObject result = new IRStackObject();
            result.Type = resultType;
            result.LinearizedTarget = new IRLinearizedLocation(IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, resultType);
            Destination = new IRLinearizedLocation(result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRUnboxInstruction(Type, Value), pNewMethod); }
    }
}
