using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRConvertCheckedInstruction : IRInstruction
    {
        public IRType Type { get; private set; }
        
        private IROverflowType mOverflowType = IROverflowType.None;
        public IROverflowType OverflowType
        {
            get { return mOverflowType; }
            private set { mOverflowType = value; }
        }

        public IRConvertCheckedInstruction(IRType pType, IROverflowType pOverflowType) : base(IROpcode.ConvertChecked)
        {
            Type = pType;
            OverflowType = pOverflowType;
        }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            Sources.Add(new IRLinearizedLocation(pStack.Pop().LinearizedTarget));

            IRStackObject result = new IRStackObject();
            result.Type = Type;
            result.LinearizedTarget = new IRLinearizedLocation(IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, Type);
            Destination = new IRLinearizedLocation(result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRConvertCheckedInstruction(Type, OverflowType), pNewMethod); }
    }
}
