using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRConvertUncheckedInstruction : IRInstruction
	{
		private IRType mType = null;
		public IRType TargetType { get { return mType; } private set { mType = value; } }

		public IRConvertUncheckedInstruction(IRType pType) : base(IROpcode.ConvertUnchecked) { TargetType = pType; }

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget));

			IRStackObject result = new IRStackObject();
			result.Type = TargetType;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
			result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, TargetType);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
			pStack.Push(result);
		}

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRConvertUncheckedInstruction(TargetType), pNewMethod); }

		public override bool Resolved { get { return TargetType.Resolved; } }
		public override void Resolve()
		{
			base.Resolve();
			TargetType.Resolve(ref mType, ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters);
		}

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
		}

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Type {0}", TargetType.ToString());
		}

		public override string ToString()
		{
			return "ConvertUnchecked " + TargetType + " " + Sources[0] + " -> " + Destination;
		}
	}
}
