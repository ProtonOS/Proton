using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRConvertCheckedInstruction : IRInstruction
    {
		private IRType mType = null;
		public IRType Type { get { return mType; } private set { mType = value; } }
        
        private IROverflowType mOverflowType = IROverflowType.None;
        public IROverflowType OverflowType
        {
            get { return mOverflowType; }
            private set { mOverflowType = value; }
        }

        public IRConvertCheckedInstruction(IRType pType, IROverflowType pOverflowType) : base(IROpcode.ConvertChecked)
        {
            Type = pType;
            OverflowType = pOverflowType;
        }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
			Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget));

            IRStackObject result = new IRStackObject();
            result.Type = Type;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, Type);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRConvertCheckedInstruction(Type, OverflowType), pNewMethod); }

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
