using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRCompareInstruction : IRInstruction
	{
		private IRCompareCondition mCompareCondition = IRCompareCondition.Equal;
		public IRCompareCondition CompareCondition
		{
			get { return mCompareCondition; }
			private set { mCompareCondition = value; }
		}

		public IRCompareInstruction(IRCompareCondition pCompareCondition) : base(IROpcode.Compare) { CompareCondition = pCompareCondition; }

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			IRStackObject value2 = pStack.Pop();
			IRStackObject value1 = pStack.Pop();

			Sources.Add(new IRLinearizedLocation(this, value1.LinearizedTarget));
			Sources.Add(new IRLinearizedLocation(this, value2.LinearizedTarget));

			IRStackObject result = new IRStackObject();
			result.Type = ParentMethod.Assembly.AppDomain.System_Int32;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
			result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, ParentMethod.Assembly.AppDomain.System_Int32);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
			pStack.Push(result);
		}

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRCompareInstruction(CompareCondition), pNewMethod); }

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
			var sA = pLIRMethod.RequestLocal(Sources[0].GetTypeOfLocation());
			Sources[0].LoadTo(pLIRMethod, sA);
			var sB = pLIRMethod.RequestLocal(Sources[1].GetTypeOfLocation());
			Sources[1].LoadTo(pLIRMethod, sB);
			var dest = pLIRMethod.RequestLocal(Destination.GetTypeOfLocation());
			LIRInstructions.CompareCondition condition = LIRInstructions.CompareCondition.Equal;
			switch (CompareCondition)
			{
				case IRCompareCondition.Equal: break;
				case IRCompareCondition.GreaterThan: 
				case IRCompareCondition.GreaterThanUnsigned:
					condition = LIRInstructions.CompareCondition.GreaterThan;
					break;
				case IRCompareCondition.LessThan:
				case IRCompareCondition.LessThanUnsigned:
					condition = LIRInstructions.CompareCondition.LessThan;
					break;
				default: throw new Exception("Invalid Compare Condition");
			}
			new LIRInstructions.Compare(pLIRMethod, sA, sB, dest, sA.Type, condition);
			pLIRMethod.ReleaseLocal(sA);
			pLIRMethod.ReleaseLocal(sB);
			Destination.StoreTo(pLIRMethod, dest);
			pLIRMethod.ReleaseLocal(dest);
		}

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("CompareCondition {0}", CompareCondition.ToString());
		}

		public override string ToString()
		{
			string cName;
			string cSym;
			switch (CompareCondition)
			{
				case IRCompareCondition.Equal:
					cName = "Equal";
					cSym = "==";
					break;
				case IRCompareCondition.GreaterThan:
					cName = "GreaterThan";
					cSym = ">";
					break;
				case IRCompareCondition.GreaterThanUnsigned:
					cName = "GreaterThan Unsigned";
					cSym = ">";
					break;
				case IRCompareCondition.LessThan:
					cName = "LessThan";
					cSym = "<";
					break;
				case IRCompareCondition.LessThanUnsigned:
					cName = "LessThan Unsigned";
					cSym = "<";
					break;
				default:
					throw new Exception("Unknown CompareCondition!");
			}
			return "Compare " + cName + " " + Sources[0] + " " + cSym + " " + Sources[1] + " -> " + Destination;
		}
	}
}
