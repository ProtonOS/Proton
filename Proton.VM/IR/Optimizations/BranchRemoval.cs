using System;
using System.Collections.Generic;
using Proton.VM.IR.Instructions;

namespace Proton.VM.IR.Optimizations
{
	public sealed class IRBranchRemovalOptimizationPass : IROptimizationPass
	{
		public override string Name { get { return "Branch Removal"; } }
		public override string Description { get { return "Removes branches to the next instruction."; } }
		public override IROptimizationPass.RunLocation Location { get { return RunLocation.BeforeSSA; } }

		public override void Run(IRMethod pMethod)
		{
			pMethod.Instructions.ImmediateRetargetModifiedInstructions = false;
			for (int i = 0; i < pMethod.Instructions.Count - 1; i++)
			{
				var curInstr = pMethod.Instructions[i];
				if (curInstr.Opcode == IROpcode.Branch)
				{
					var curBranchInstr = (IRBranchInstruction)curInstr;
					if (curBranchInstr.BranchCondition == IRBranchCondition.Always && curBranchInstr.TargetIRInstruction.IRIndex == curBranchInstr.IRIndex + 1)
					{
						pMethod.Instructions[i] = new IRNopInstruction();
					}
				}
			}
			pMethod.Instructions.FixModifiedTargetInstructions();
			pMethod.Instructions.ImmediateRetargetModifiedInstructions = true;
		}
	}
}
