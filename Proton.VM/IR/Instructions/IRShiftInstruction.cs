using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRShiftInstruction : IRInstruction
	{
		private IRShiftType mShiftType = IRShiftType.Left;
		public IRShiftType ShiftType 
		{
			get { return mShiftType; }
			private set { mShiftType = value; }
		}

		public IRShiftInstruction(IRShiftType pShiftType) : base(IROpcode.Shift) { ShiftType = pShiftType; }

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			IRStackObject shiftAmount = pStack.Pop();
			IRStackObject value = pStack.Pop();

			Sources.Add(new IRLinearizedLocation(this, value.LinearizedTarget));
			Sources.Add(new IRLinearizedLocation(this, shiftAmount.LinearizedTarget));

			IRType resultType = ParentMethod.Assembly.AppDomain.ShiftNumericResult(value.Type, shiftAmount.Type);
			IRStackObject result = new IRStackObject();
			result.Type = resultType;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
			result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, resultType);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
			pStack.Push(result);
		}

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRShiftInstruction(ShiftType), pNewMethod); }

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
			var sA = pLIRMethod.RequestLocal(Sources[0].GetTypeOfLocation());
			Sources[0].LoadTo(pLIRMethod, sA);
			var sB = pLIRMethod.RequestLocal(Sources[1].GetTypeOfLocation());
			Sources[1].LoadTo(pLIRMethod, sB);
			var dest = pLIRMethod.RequestLocal(Destination.GetTypeOfLocation());
			LIRInstructions.MathOperation shiftOp = LIRInstructions.MathOperation.ShiftLeft;
			switch (ShiftType)
			{
				case IRShiftType.Left: shiftOp = LIRInstructions.MathOperation.ShiftLeft; break;
				case IRShiftType.Right: shiftOp = LIRInstructions.MathOperation.ShiftRight; break;
				case IRShiftType.RightSignExtended: shiftOp = LIRInstructions.MathOperation.ShiftRightSignExtended; break;
				default: throw new Exception();
			}
			new LIRInstructions.Math(pLIRMethod, sA, sB, dest, shiftOp, dest.Type);
			pLIRMethod.ReleaseLocal(sA);
			pLIRMethod.ReleaseLocal(sB);
			Destination.StoreTo(pLIRMethod, dest);
			pLIRMethod.ReleaseLocal(dest);
		}

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("ShiftType {0}", ShiftType.ToString());
		}

		public override string ToString()
		{
			string shiftSym;
			switch (ShiftType)
			{
				case IRShiftType.Left: shiftSym = "<<"; break;
				case IRShiftType.Right: shiftSym = ">>"; break;
				case IRShiftType.RightSignExtended: shiftSym = ">>>"; break;
				default:
					throw new Exception("Unknown ShiftType!");
			}
			return "Shift " + Sources[0] + " " + shiftSym + " " + Sources[1] + " -> " + Destination;
		}
	}
}
