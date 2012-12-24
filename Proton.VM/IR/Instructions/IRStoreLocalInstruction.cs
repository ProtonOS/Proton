using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRStoreLocalInstruction : IRInstruction
    {
        public int LocalIndex { get; private set; }

        public IRStoreLocalInstruction(int pLocalIndex) : base(IROpcode.StoreLocal) { LocalIndex = pLocalIndex; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
			Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget));

			Destination = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
            Destination.Local.LocalIndex = LocalIndex;
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRStoreLocalInstruction(LocalIndex), pNewMethod); }

        public override IRInstruction Transform() { return new IRMoveInstruction(this); }

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("LocalIndex {0}", LocalIndex);
		}
	}
}
