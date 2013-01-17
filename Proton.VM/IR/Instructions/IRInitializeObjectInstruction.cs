using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRInitializeObjectInstruction : IRInstruction
	{
		private IRType mType = null;
		public IRType Type { get { return mType; } private set { mType = value; } }

		public IRInitializeObjectInstruction(IRType pType) : base(IROpcode.InitializeObject) { Type = pType; }

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget));
		}

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRInitializeObjectInstruction(Type), pNewMethod); }

		public override bool Resolved { get { return Type.Resolved; } }
		public override void Resolve()
		{
			base.Resolve();
			Type.Resolve(ref mType, ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters);
		}

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
			var s = pLIRMethod.RequestLocal(Sources[0].GetTypeOfLocation());
			Sources[0].LoadTo(pLIRMethod, s);
			var d = pLIRMethod.RequestLocal(Type);
			new LIRInstructions.Move(pLIRMethod, Type.ToLIRType().Empty, d, Type);
			new LIRInstructions.Move(pLIRMethod, d, new Indirect(s), Type);
			pLIRMethod.ReleaseLocal(s);
			pLIRMethod.ReleaseLocal(d);
		}

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Type {0}", Type.ToString());
		}

		public override string ToString()
		{
			return String.Format("InitializeObject {0} {1}", Type, Sources[0]);
		}
	}
}
