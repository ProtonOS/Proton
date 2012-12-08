using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRLoadFunctionInstruction : IRInstruction
    {
        public IRMethod Target { get; private set; }
        public bool Virtual { get; private set; }

        public IRLoadFunctionInstruction(IRMethod pTarget, bool pVirtual) : base(IROpcode.LoadFunction)
        {
            Target = pTarget;
            Virtual = pVirtual;
        }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            IRLinearizedLocation value = new IRLinearizedLocation(IRLinearizedLocationType.FunctionAddress);
            value.FunctionAddress.Method = Target;
            value.FunctionAddress.Virtual = Virtual;
            Sources.Add(value);

            IRStackObject result = new IRStackObject();
            result.Type = ParentMethod.Assembly.AppDomain.System_IntPtr;
            result.LinearizedTarget = new IRLinearizedLocation(IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, ParentMethod.Assembly.AppDomain.System_IntPtr);
            Destination = new IRLinearizedLocation(result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRLoadFunctionInstruction(Target, Virtual), pNewMethod); }

        public override IRInstruction Transform() { return new IRMoveInstruction(this); }
    }
}
