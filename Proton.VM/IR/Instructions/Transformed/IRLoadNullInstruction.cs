using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRLoadNullInstruction : IRInstruction
    {
        public IRLoadNullInstruction() : base(IROpcode.LoadNull) { }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            Sources.Add(new IRLinearizedLocation(this, IRLinearizedLocationType.Null));

            IRStackObject result = new IRStackObject();
            result.Type = ParentMethod.Assembly.AppDomain.System_Object;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, ParentMethod.Assembly.AppDomain.System_Object);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRLoadNullInstruction(), pNewMethod); }

        public override IRInstruction Transform() { return new IRMoveInstruction(this); }

		public override void ConvertToLIR(LIRMethod pLIRMethod) { }
	}
}
