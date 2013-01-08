using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRBranchInstruction : IRInstruction
	{
		private IRBranchCondition mBranchCondition = IRBranchCondition.Always;
		public IRBranchCondition BranchCondition
		{
			get { return mBranchCondition; }
			set { mBranchCondition = value; }
		}
		
		public int TargetILOffset { get; set; }
		public IRInstruction TargetIRInstruction { get; set; }
		public int StackDepth { get; set; }

		public IRBranchInstruction(IRBranchCondition pBranchCondition, int pTargetILOffset) : base(IROpcode.Branch)
		{
			BranchCondition = pBranchCondition;
			TargetILOffset = pTargetILOffset;
		}

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			switch (BranchCondition)
			{
				case IRBranchCondition.Always:
					//if (pStack.Count > 0)
					//{
					//	int sd = Method.StackDepths.Pop();
					//	while (pStack.Count > sd)
					//		pStack.Pop();
					//}
					break;
				case IRBranchCondition.False:
				case IRBranchCondition.True: Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget)); break;
				case IRBranchCondition.Equal:
				case IRBranchCondition.GreaterOrEqual:
				case IRBranchCondition.GreaterOrEqualUnsigned:
				case IRBranchCondition.Greater:
				case IRBranchCondition.GreaterUnsigned:
				case IRBranchCondition.LessOrEqual:
				case IRBranchCondition.LessOrEqualUnsigned:
				case IRBranchCondition.Less:
				case IRBranchCondition.LessUnsigned:
				case IRBranchCondition.NotEqualUnsigned:
					{
						IRStackObject value2 = pStack.Pop();
						IRStackObject value1 = pStack.Pop();
						Sources.Add(new IRLinearizedLocation(this, value1.LinearizedTarget));
						Sources.Add(new IRLinearizedLocation(this, value2.LinearizedTarget));
						break;
					}
				default:
					throw new Exception("Unknown BranchCondition!");
			}
		}

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRBranchInstruction(BranchCondition, TargetILOffset) { TargetIRInstruction = this.TargetIRInstruction }, pNewMethod); }

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
		}

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("BranchCondition {0} -> {1}", BranchCondition, TargetIRInstruction.IRIndex);
		}

		public override string ToString()
		{
			string branchName;
			string branchSym;
			switch (BranchCondition)
			{
				case IRBranchCondition.Always:
					return "Branch Always -> " + TargetIRInstruction.IRIndex;

				case IRBranchCondition.False:
					return "Branch False " + Sources[0] + " -> " + TargetIRInstruction.IRIndex;
				case IRBranchCondition.True:
					return "Branch True " + Sources[0] + " -> " + TargetIRInstruction.IRIndex;

				case IRBranchCondition.Equal:
					branchName = "Equal";
					branchSym = "==";
					break;
				case IRBranchCondition.GreaterOrEqual:
					branchName = "GreaterOrEqual";
					branchSym = ">=";
					break;
				case IRBranchCondition.GreaterOrEqualUnsigned:
					branchName = "GreaterOrEqual Unsigned";
					branchSym = ">=";
					break;
				case IRBranchCondition.Greater:
					branchName = "Greater";
					branchSym = ">";
					break;
				case IRBranchCondition.GreaterUnsigned:
					branchName = "Greater Unsigned";
					branchSym = ">";
					break;
				case IRBranchCondition.LessOrEqual:
					branchName = "LessOrEqual";
					branchSym = "<=";
					break;
				case IRBranchCondition.LessOrEqualUnsigned:
					branchName = "LessOrEqual Unsigned";
					branchSym = "<=";
					break;
				case IRBranchCondition.Less:
					branchName = "Less";
					branchSym = "<";
					break;
				case IRBranchCondition.LessUnsigned:
					branchName = "Less Unsigned";
					branchSym = "<";
					break;
				case IRBranchCondition.NotEqualUnsigned:
					branchName = "NotEqual Unsigned";
					branchSym = "!=";
					break;

				default:
					throw new Exception("Unknown BranchCondition!");
			}
			return "Branch " + branchName + " " + Sources[0] + " " + branchSym + " " + Sources[1] + " -> " + TargetIRInstruction.IRIndex;
		}
	}
}
