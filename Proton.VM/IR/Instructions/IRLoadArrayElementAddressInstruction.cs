using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRLoadArrayElementAddressInstruction : IRInstruction
    {
		private IRType mType = null;
		public IRType Type { get { return mType; } private set { mType = value; } }

        public IRLoadArrayElementAddressInstruction(IRType pType) : base(IROpcode.LoadArrayElementAddress) { Type = pType; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
			IRLinearizedLocation source = new IRLinearizedLocation(this, IRLinearizedLocationType.ArrayElementAddress);
			source.ArrayElementAddress.IndexLocation = new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget);
			var arraySource = pStack.Pop();
			source.ArrayElementAddress.ArrayLocation = new IRLinearizedLocation(this, arraySource.LinearizedTarget);
			if (Type == null)
			{
				Type = arraySource.Type.ArrayElementType;
			}
			if (Type == null) throw new Exception();
            source.ArrayElementAddress.ElementType = Type;
            Sources.Add(source);

            IRStackObject result = new IRStackObject();
            result.Type = ParentMethod.Assembly.AppDomain.System_IntPtr;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, ParentMethod.Assembly.AppDomain.System_IntPtr);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRLoadArrayElementAddressInstruction(Type), pNewMethod); }

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
