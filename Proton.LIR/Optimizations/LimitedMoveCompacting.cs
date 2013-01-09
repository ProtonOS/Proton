using System;
using System.Collections.Generic;

namespace Proton.LIR.Optimizations
{
	internal sealed class LIRLimitedMoveCompactingOptimizationPass : LIROptimizationPass
	{
		public override void Run(LIRMethod method)
		{
			for (int i = 0; i < method.mInstructions.Count; i++)
			{
				var curInstr = method.mInstructions[i];
				if (curInstr.OpCode == LIROpCode.Move)
				{
					var curMove = (Instructions.Move)curInstr;
					if (curMove.Source.SourceType == SourceType.Local)
					{
						var loc = (LIRLocal)curMove.Source;
						if (loc.Dynamic && loc.AssignedValue != null && loc.AssignedValue.SourceType == SourceType.Local)
						{
							curMove.Source = loc.AssignedValue;
							method.mInstructions[loc.AssignedAt] = new Instructions.Dead(loc.AssignedAt);
						}
					}
					if (curMove.Destination.DestinationType == DestinationType.Local)
					{
						var loc = (LIRLocal)curMove.Destination;
						if (loc.Dynamic)
						{
							loc.AssignedAt = i;
							loc.AssignedValue = curMove.Source;
						}
					}
				}
				else
				{
					method.Locals.ForEach(m => { m.AssignedAt = -1; m.AssignedValue = null; });
				}
			}
		}
	}
}
