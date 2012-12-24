using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRLoadReal64Instruction : IRInstruction
    {
        public double Value { get; private set; }

        public IRLoadReal64Instruction(double pValue) : base(IROpcode.LoadReal64) { Value = pValue; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
			IRLinearizedLocation value = new IRLinearizedLocation(this, IRLinearizedLocationType.ConstantR8);
            value.ConstantR8.Value = Value;
            Sources.Add(value);

            IRStackObject result = new IRStackObject();
            result.Type = ParentMethod.Assembly.AppDomain.System_Double;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, ParentMethod.Assembly.AppDomain.System_Double);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRLoadReal64Instruction(Value), pNewMethod); }

        public override IRInstruction Transform() { return new IRMoveInstruction(this); }

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Value {0}", Value);
		}
	}
}
