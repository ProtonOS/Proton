using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRCallInstruction : IRInstruction
    {
        public IRMethod Target { get; private set; }
        public bool Virtual { get; private set; }

        public IRCallInstruction(IRMethod pTarget, bool pVirtual) : base(IROpcode.Call)
        {
            Target = pTarget;
            Virtual = pVirtual;
        }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            for (int count = 0; count < Target.Parameters.Count; ++count) Sources.Add(new IRLinearizedLocation(pStack.Pop().LinearizedTarget));

            if (Target.ReturnType != null)
            {
                IRStackObject returned = new IRStackObject();
				IRType retType = Target.ReturnType;

				if (retType.IsTemporaryVar)
					retType = Target.ParentType.GenericParameters[retType.TemporaryVarOrMVarIndex];
				else if (retType.IsTemporaryMVar)
					retType = Target.GenericParameters[retType.TemporaryVarOrMVarIndex];

				returned.Type = retType;
                returned.LinearizedTarget = new IRLinearizedLocation(IRLinearizedLocationType.Local);
                returned.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, retType);
                Destination = new IRLinearizedLocation(returned.LinearizedTarget);
                pStack.Push(returned);
            }
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRCallInstruction(Target, Virtual), pNewMethod); }
    }
}
