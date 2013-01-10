using System;
using System.Collections.Generic;

namespace Proton.LIR.Optimizations
{
	internal sealed class LIRDeadInstructionDestructionOptimizationPass : LIROptimizationPass
	{
		public override void Run(LIRMethod method)
		{
			List<LIRInstruction> instrs = new List<LIRInstruction>(method.mInstructions.Count);
			foreach (var i in method.mInstructions)
			{
				if (i.OpCode == LIROpCode.Dead)
					continue;
				i.Index = instrs.Count;
				instrs.Add(i);
			}
			instrs.TrimExcess();
			method.mInstructions = instrs;
		}
	}
}
