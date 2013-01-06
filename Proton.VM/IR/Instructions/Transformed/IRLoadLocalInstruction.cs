using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRLoadLocalInstruction : IRInstruction
    {
        public int LocalIndex { get; private set; }

        public IRLoadLocalInstruction(int pLocalIndex) : base(IROpcode.LoadLocal) { LocalIndex = pLocalIndex; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
			IRLinearizedLocation source = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
            source.Local.LocalIndex = LocalIndex;
            Sources.Add(source);

            IRStackObject result = new IRStackObject();
            result.Type = ParentMethod.Locals[(int)LocalIndex].Type;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, ParentMethod.Locals[(int)LocalIndex].Type);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRLoadLocalInstruction(LocalIndex), pNewMethod); }

        public override IRInstruction Transform() { return new IRMoveInstruction(this); }

		public override void ConvertToLIR(LIRMethod pLIRMethod) { }

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("LocalIndex {0}", LocalIndex);
		}
	}
}
