using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRLoadReal32Instruction : IRInstruction
    {
        public float Value { get; private set; }

        public IRLoadReal32Instruction(float pValue) : base(IROpcode.LoadReal32) { Value = pValue; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
			IRLinearizedLocation value = new IRLinearizedLocation(this, IRLinearizedLocationType.ConstantR4);
            value.ConstantR4.Value = Value;
            Sources.Add(value);

            IRStackObject result = new IRStackObject();
            result.Type = ParentMethod.Assembly.AppDomain.System_Single;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, ParentMethod.Assembly.AppDomain.System_Single);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRLoadReal32Instruction(Value), pNewMethod); }

        public override IRInstruction Transform() { return new IRMoveInstruction(this); }

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Value {0}", Value);
		}
	}
}
