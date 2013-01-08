using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRCastInstruction : IRInstruction
	{
		private IRType mType = null;
		public IRType Type { get { return mType; } private set { mType = value; } }
		public bool ThrowExceptionOnFailure { get; private set; }

		public IRCastInstruction(IRType pType, bool pThrowExceptionOnFailure) : base(IROpcode.Cast)
		{
			Type = pType;
			ThrowExceptionOnFailure = pThrowExceptionOnFailure;
		}

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget));

			IRStackObject result = new IRStackObject();
			result.Type = Type;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
			result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, Type);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
			pStack.Push(result);
		}

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRCastInstruction(Type, ThrowExceptionOnFailure), pNewMethod); }

		public override bool Resolved { get { return Type.Resolved; } }
		public override void Resolve()
		{
			base.Resolve();
			Type.Resolve(ref mType, ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters);
		}

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
		}

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Type {0}", Type.ToString());
			pWriter.WriteLine("ThrowExceptionOnFailure {0}", ThrowExceptionOnFailure);
		}

		public override string ToString()
		{
			return "Cast " + (ThrowExceptionOnFailure ? "ThrowOnFail " : "") + Type + " " + Sources[0] + " -> " + Destination;
		}
	}
}
