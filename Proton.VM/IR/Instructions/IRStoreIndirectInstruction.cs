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
			Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget));

			Destination = new IRLinearizedLocation(this, IRLinearizedLocationType.Indirect);
			var addressLocation = pStack.Pop();
			Destination.Indirect.AddressLocation = new IRLinearizedLocation(this, addressLocation.LinearizedTarget);
			if (Type == null)
			{
				if (addressLocation.Type.IsManagedPointerType)
					Type = addressLocation.Type.ManagedPointerType;
				else
					Type = addressLocation.Type.UnmanagedPointerType;
			}
			if (Type == null) throw new Exception();
			Destination.Indirect.Type = Type;
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
