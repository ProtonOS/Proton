using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRJumpInstruction : IRInstruction
	{
		private IRMethod mTarget = null;
		public IRMethod Target { get { return mTarget; } private set { mTarget = value; } }

		public IRJumpInstruction(IRMethod pTarget) : base(IROpcode.Jump) { Target = pTarget; }

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			if (pStack.Count > 0) throw new OverflowException();
		}

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRJumpInstruction(Target), pNewMethod); }

		public override bool Resolved { get { return Target.Resolved; } }
		public override void Resolve()
		{
			base.Resolve();
			Target.Resolve(ref mTarget, ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters);
		}

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
		}

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Target {0}", Target.ToString());
		}
	}
}
