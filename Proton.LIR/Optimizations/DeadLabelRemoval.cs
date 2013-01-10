using System;
using System.Collections.Generic;

namespace Proton.LIR.Optimizations
{
	internal sealed class LIRDeadLabelRemovalOptimizationPass : LIROptimizationPass
	{
		public override void Run(LIRMethod method)
		{
			for (int i = 0; i < method.mInstructions.Count; i++)
			{
				var curInstr = method.mInstructions[i];
				if (curInstr.OpCode == LIROpCode.Label && ((Label)curInstr).References == 0)
					method.mInstructions[i] = new Instructions.Dead(i);
			}
		}
	}
}
