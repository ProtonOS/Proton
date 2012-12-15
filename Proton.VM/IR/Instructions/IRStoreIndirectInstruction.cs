using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRStoreIndirectInstruction : IRInstruction
    {
		private IRType mType = null;
		public IRType Type { get { return mType; } private set { mType = value; } }

        public IRStoreIndirectInstruction(IRType pType) : base(IROpcode.StoreIndirect) { Type = pType; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            Sources.Add(new IRLinearizedLocation(pStack.Pop().LinearizedTarget));

            Destination = new IRLinearizedLocation(IRLinearizedLocationType.Indirect);
            Destination.Indirect.Type = Type;
            Destination.Indirect.AddressLocation = new IRLinearizedLocation(pStack.Pop().LinearizedTarget);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRStoreIndirectInstruction(Type), pNewMethod); }

        public override IRInstruction Transform() { return new IRMoveInstruction(this); }

		public override bool Resolved { get { return Type.Resolved; } }
		public override void Resolve()
		{
			base.Resolve();
			Type.Resolve(ref mType, ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters);
		}

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Type {0}", Type.ToString());
		}
	}
}
