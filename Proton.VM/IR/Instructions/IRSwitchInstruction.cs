using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRSwitchInstruction : IRInstruction
    {
        public int[] TargetILOffsets = null;
        public IRInstruction[] TargetIRInstructions = null;

        public IRSwitchInstruction(int[] pTargetILOffsets) : base(IROpcode.Switch) { TargetILOffsets = pTargetILOffsets; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            Sources.Add(new IRLinearizedLocation(pStack.Pop().LinearizedTarget));
        }

        public override IRInstruction Clone(IRMethod pNewMethod)
        {
            IRInstruction[] newTargetIRInstructions = (IRInstruction[])TargetIRInstructions.Clone();
            return CopyTo(new IRSwitchInstruction(TargetILOffsets) { TargetIRInstructions = newTargetIRInstructions }, pNewMethod);
        }
    }
}
