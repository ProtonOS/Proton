using System;
using System.Collections.Generic;

namespace Proton.LIR.Optimizations
{
	internal sealed class LIRDeadLabelRemovalOptimizationPass : LIROptimizationPass
	{
		public override void Run(LIRMethod method)
		{
			List<LIRInstruction> instrs = new List<LIRInstruction>(method.mInstructions.Count);
			foreach (var i in method.mInstructions)
			{
				if (i.OpCode == LIROpCode.Label)
				{
					if (((Label)i).References == 0)
						continue;
				}
				else if (i.OpCode == LIROpCode.Dead)
					continue;
				i.Index = instrs.Count;
				instrs.Add(i);
			}
			instrs.TrimExcess();
			method.mInstructions = instrs;
		}
	}
}
