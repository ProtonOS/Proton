using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRCopyObjectInstruction : IRInstruction
    {
		private IRType mType = null;
		public IRType Type { get { return mType; } private set { mType = value; } }

        public IRCopyObjectInstruction(IRType pType) : base(IROpcode.CopyObject) { Type = pType; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            IRStackObject srcAddr = pStack.Pop();
            IRStackObject destAddr = pStack.Pop();
			Sources.Add(new IRLinearizedLocation(this, destAddr.LinearizedTarget));
			Sources.Add(new IRLinearizedLocation(this, srcAddr.LinearizedTarget));
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRCopyObjectInstruction(Type), pNewMethod); }

		public override bool Resolved { get { return Type.Resolved; } }
		public override void Resolve()
		{
			base.Resolve();
			Type.Resolve(ref mType, ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters);
		}

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
		}

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Type {0}", Type.ToString());
		}
	}
}
