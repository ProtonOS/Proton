using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRShiftInstruction : IRInstruction
    {
        private IRShiftType mShiftType = IRShiftType.Left;
        public IRShiftType ShiftType 
        {
            get { return mShiftType; }
            private set { mShiftType = value; }
        }

        public IRShiftInstruction(IRShiftType pShiftType) : base(IROpcode.Shift) { ShiftType = pShiftType; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            IRStackObject shiftAmount = pStack.Pop();
            IRStackObject value = pStack.Pop();

            Sources.Add(new IRLinearizedLocation(value.LinearizedTarget));
            Sources.Add(new IRLinearizedLocation(shiftAmount.LinearizedTarget));

            IRType resultType = ParentMethod.Assembly.AppDomain.ShiftNumericResult(value.Type, shiftAmount.Type);
            IRStackObject result = new IRStackObject();
            result.Type = resultType;
            result.LinearizedTarget = new IRLinearizedLocation(IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, resultType);
            Destination = new IRLinearizedLocation(result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRShiftInstruction(ShiftType), pNewMethod); }
    }
}
