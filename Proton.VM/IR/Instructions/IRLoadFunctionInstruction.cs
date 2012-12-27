using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRLoadFunctionInstruction : IRInstruction
    {
		private IRMethod mTarget = null;
		public IRMethod Target { get { return mTarget; } private set { mTarget = value; } }
        public bool Virtual { get; private set; }

        public IRLoadFunctionInstruction(IRMethod pTarget, bool pVirtual) : base(IROpcode.LoadFunction)
        {
            Target = pTarget;
            Virtual = pVirtual;
        }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
			IRLinearizedLocation value = new IRLinearizedLocation(this, IRLinearizedLocationType.FunctionAddress);
			if (Virtual) value.FunctionAddress.InstanceLocation = pStack.Pop().LinearizedTarget;
            value.FunctionAddress.Method = Target;
            value.FunctionAddress.Virtual = Virtual;
            Sources.Add(value);

            IRStackObject result = new IRStackObject();
            result.Type = ParentMethod.Assembly.AppDomain.System_IntPtr;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, ParentMethod.Assembly.AppDomain.System_IntPtr);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRLoadFunctionInstruction(Target, Virtual), pNewMethod); }

        public override IRInstruction Transform() { return new IRMoveInstruction(this); }

		public override bool Resolved { get { return Target.Resolved; } }
		public override void Resolve()
		{
			base.Resolve();
			Target.Resolve(ref mTarget, ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters);
		}

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Target {0}", Target.ToString());
			pWriter.WriteLine("Virtual {0}", Virtual);
		}
	}
}
