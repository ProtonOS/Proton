using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRLoadStringInstruction : IRInstruction
    {
        public string Value { get; private set; }

        public IRLoadStringInstruction(string pValue) : base(IROpcode.LoadString) { Value = pValue; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
			IRLinearizedLocation value = new IRLinearizedLocation(this, IRLinearizedLocationType.String);
            value.String.Value = Value;
            Sources.Add(value);

            IRStackObject result = new IRStackObject();
            result.Type = ParentMethod.Assembly.AppDomain.System_String;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, ParentMethod.Assembly.AppDomain.System_String);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRLoadStringInstruction(Value), pNewMethod); }

        public override IRInstruction Transform() { return new IRMoveInstruction(this); }

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Value {0}", Value);
		}
	}
}
