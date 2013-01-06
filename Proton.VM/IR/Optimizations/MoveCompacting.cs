using System;
using System.Collections.Generic;
using Proton.VM.IR.Instructions;

namespace Proton.VM.IR.Optimizations
{
	public sealed class IRNopKillingOptimizationPass : IROptimizationPass
	{
		public override string Name { get { return "Move Compacting"; } }
		public override string Description { get { return "Compacts a move to a move into a single move."; } }
		public override IROptimizationPass.RunLocation Location { get { return RunLocation.DuringSSA; } }

		public override void Run(IRMethod pMethod)
		{
			pMethod.Instructions.ImmediateRetargetModifiedInstructions = false;
			for (int i = 0; i < pMethod.Instructions.Count - 1; i++)
			{
				var curInstr = pMethod.Instructions[i];
			}

			pMethod.Instructions.FixModifiedTargetInstructions();
			pMethod.Instructions.ImmediateRetargetModifiedInstructions = true;
		}
	}
}
