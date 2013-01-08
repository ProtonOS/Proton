using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRStoreParameterInstruction : IRInstruction
    {
        public int ParameterIndex = 0;

        public IRStoreParameterInstruction(int pParameterIndex) : base(IROpcode.StoreParameter) { ParameterIndex = pParameterIndex; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
			Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget));

			Destination = new IRLinearizedLocation(this, IRLinearizedLocationType.Parameter);
            Destination.Parameter.ParameterIndex = ParameterIndex;
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRStoreParameterInstruction(ParameterIndex), pNewMethod); }

        public override IRInstruction Transform() { return new IRMoveInstruction(this); }

		public override void ConvertToLIR(LIRMethod pLIRMethod) { }

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Parameter {0}", ParameterIndex);
		}
	}
}
