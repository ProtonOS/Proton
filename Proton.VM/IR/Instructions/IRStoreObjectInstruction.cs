using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRStoreObjectInstruction : IRInstruction
    {
		private IRType mType = null;
		public IRType Type { get { return mType; } private set { mType = value; } }

        public IRStoreObjectInstruction(IRType pType) : base(IROpcode.StoreObject) { Type = pType; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
			Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget));

			Destination = new IRLinearizedLocation(this, IRLinearizedLocationType.Indirect);
            Destination.Indirect.Type = Type;
			Destination.Indirect.AddressLocation = new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRStoreObjectInstruction(Type), pNewMethod); }

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
