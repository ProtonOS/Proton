using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRLoadRuntimeHandleInstruction : IRInstruction
    {
        public IRType TargetType { get; private set; }
        public IRMethod TargetMethod { get; private set; }
        public IRField TargetField { get; private set; }

        public IRLoadRuntimeHandleInstruction(IRType pTargetType, IRMethod pTargetMethod, IRField pTargetField) : base(IROpcode.LoadRuntimeHandle)
        {
            TargetType = pTargetType;
            TargetMethod = pTargetMethod;
            TargetField = pTargetField;
        }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            IRType handleType = null;
            if (TargetType != null) handleType = ParentMethod.Assembly.AppDomain.System_RuntimeTypeHandle;
            else if (TargetMethod != null) handleType = ParentMethod.Assembly.AppDomain.System_RuntimeMethodHandle;
            else if (TargetField != null) handleType = ParentMethod.Assembly.AppDomain.System_RuntimeFieldHandle;
            else throw new NullReferenceException();

            IRLinearizedLocation value = new IRLinearizedLocation(IRLinearizedLocationType.RuntimeHandle);
            value.RuntimeHandle.HandleType = handleType;
            value.RuntimeHandle.TargetType = TargetType;
            value.RuntimeHandle.TargetMethod = TargetMethod;
            value.RuntimeHandle.TargetField = TargetField;
            Sources.Add(value);

            IRStackObject result = new IRStackObject();
            result.Type = handleType;
            result.LinearizedTarget = new IRLinearizedLocation(IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, handleType);
            Destination = new IRLinearizedLocation(result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRLoadRuntimeHandleInstruction(TargetType, TargetMethod, TargetField), pNewMethod); }

        public override IRInstruction Transform() { return new IRMoveInstruction(this); }
    }
}
