using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRLeaveInstruction : IRInstruction
    {
        public int TargetILOffset { get; private set; }
        public IRInstruction TargetIRInstruction { get; set; }

        public IRLeaveInstruction(int pTargetILOffset) : base(IROpcode.Leave) { TargetILOffset = pTargetILOffset; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            pStack.Clear();
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRLeaveInstruction(TargetILOffset) { TargetIRInstruction = this.TargetIRInstruction }, pNewMethod); }
    }
}
