using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRAddInstruction : IRInstruction
    {
        private IROverflowType mOverflowType = IROverflowType.None;
        public IROverflowType OverflowType 
        {
            get { return mOverflowType; }
            private set { mOverflowType = value; }
        }

        public IRAddInstruction(IROverflowType pOverflowType) : base(IROpcode.Add) { OverflowType = pOverflowType; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            IRStackObject value2 = pStack.Pop();
            IRStackObject value1 = pStack.Pop();

            Sources.Add(new IRLinearizedLocation(value1.LinearizedTarget));
            Sources.Add(new IRLinearizedLocation(value2.LinearizedTarget));

            IRType resultType = ParentMethod.Assembly.AppDomain.BinaryNumericResult(value1.Type, value2.Type);
            IRStackObject result = new IRStackObject();
            result.Type = resultType;
            result.LinearizedTarget = new IRLinearizedLocation(IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, resultType);
            Destination = new IRLinearizedLocation(result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRAddInstruction(OverflowType), pNewMethod); }

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("OverflowType {0}", OverflowType);
		}
	}
}
