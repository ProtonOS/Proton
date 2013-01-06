using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRLoadIndirectInstruction : IRInstruction
    {
		private IRType mType = null;
		public IRType Type { get { return mType; } private set { mType = value; } }

        public IRLoadIndirectInstruction(IRType pType) : base(IROpcode.LoadIndirect) { Type = pType; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
			IRLinearizedLocation source = new IRLinearizedLocation(this, IRLinearizedLocationType.Indirect);
			var addressLocation = pStack.Pop();
			source.Indirect.AddressLocation = new IRLinearizedLocation(this, addressLocation.LinearizedTarget);
			if (Type == null)
			{
				if (addressLocation.Type.IsManagedPointerType)
					Type = addressLocation.Type.ManagedPointerType;
				else
					Type = addressLocation.Type.UnmanagedPointerType;
			}
			if (Type == null) throw new Exception();
			source.Indirect.Type = Type;
            Sources.Add(source);

            IRStackObject result = new IRStackObject();
            result.Type = Type;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, Type);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRLoadIndirectInstruction(Type), pNewMethod); }

        public override IRInstruction Transform() { return new IRMoveInstruction(this); }

		public override bool Resolved { get { return Type.Resolved; } }
		public override void Resolve()
		{
			base.Resolve();
			Type.Resolve(ref mType, ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters);
		}

		public override void ConvertToLIR(LIRMethod pLIRMethod) { }

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Type {0}", Type.ToString());
		}
	}
}
