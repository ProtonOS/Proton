using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRLoadInteger32Instruction : IRInstruction
	{
		public int Value { get; private set; }

		public IRLoadInteger32Instruction(int pValue) : base(IROpcode.LoadInteger32) { Value = pValue; }

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			IRLinearizedLocation value = new IRLinearizedLocation(this, IRLinearizedLocationType.ConstantI4);
			value.ConstantI4.Value = Value;
			Sources.Add(value);

			IRStackObject result = new IRStackObject();
			result.Type = ParentMethod.Assembly.AppDomain.System_Int32;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
			result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, ParentMethod.Assembly.AppDomain.System_Int32);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
			pStack.Push(result);
		}

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRLoadInteger32Instruction(Value), pNewMethod); }

		public override IRInstruction Transform() { return new IRMoveInstruction(this); }

		public override void ConvertToLIR(LIRMethod pLIRMethod) { }

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Value {0}", Value);
		}
	}
}
