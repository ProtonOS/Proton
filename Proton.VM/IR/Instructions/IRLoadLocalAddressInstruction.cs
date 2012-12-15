using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRLoadLocalAddressInstruction : IRInstruction
    {
        public int LocalIndex { get; private set; }

        public IRLoadLocalAddressInstruction(int pLocalIndex) : base(IROpcode.LoadLocalAddress) { LocalIndex = pLocalIndex; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            IRLinearizedLocation source = new IRLinearizedLocation(IRLinearizedLocationType.LocalAddress);
            source.LocalAddress.LocalIndex = LocalIndex;
            Sources.Add(source);

            IRStackObject result = new IRStackObject();
            result.Type = ParentMethod.Assembly.AppDomain.System_IntPtr;
            result.LinearizedTarget = new IRLinearizedLocation(IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, ParentMethod.Assembly.AppDomain.System_IntPtr);
            Destination = new IRLinearizedLocation(result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRLoadLocalAddressInstruction(LocalIndex), pNewMethod); }

        public override IRInstruction Transform() { return new IRMoveInstruction(this); }

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("LocalIndex {0}", LocalIndex);
		}
	}
}
