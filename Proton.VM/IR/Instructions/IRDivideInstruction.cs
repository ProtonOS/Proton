using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRDivideInstruction : IRInstruction
	{
		private IROverflowType mOverflowType = IROverflowType.None;
		public IROverflowType OverflowType
		{
			get { return mOverflowType; }
			private set { mOverflowType = value; }
		}

		public IRDivideInstruction(IROverflowType pOverflowType) : base(IROpcode.Divide) { OverflowType = pOverflowType; }

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			IRStackObject value2 = pStack.Pop();
			IRStackObject value1 = pStack.Pop();

			Sources.Add(new IRLinearizedLocation(this, value1.LinearizedTarget));
			Sources.Add(new IRLinearizedLocation(this, value2.LinearizedTarget));

			IRType resultType = ParentMethod.Assembly.AppDomain.BinaryNumericResult(value1.Type, value2.Type);
			IRStackObject result = new IRStackObject();
			result.Type = resultType;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
			result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, resultType);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
			pStack.Push(result);
		}

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRDivideInstruction(OverflowType), pNewMethod); }

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
			var sA = pLIRMethod.RequestLocal(Sources[0].GetTypeOfLocation());
			Sources[0].LoadTo(pLIRMethod, sA);
			var sB = pLIRMethod.RequestLocal(Sources[1].GetTypeOfLocation());
			Sources[1].LoadTo(pLIRMethod, sB);
			var dest = pLIRMethod.RequestLocal(Destination.GetTypeOfLocation());
			new LIRInstructions.Math(pLIRMethod, sA, sB, dest, LIRInstructions.MathOperation.Divide, dest.Type);
			pLIRMethod.ReleaseLocal(sA);
			pLIRMethod.ReleaseLocal(sB);
			Destination.StoreTo(pLIRMethod, dest);
			pLIRMethod.ReleaseLocal(dest);
		}

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("OverflowType {0}", OverflowType.ToString());
		}

		public override string ToString()
		{
			return "Divide " + Sources[0] + " / " + Sources[1] + " -> " + Destination;
		}
	}
}
