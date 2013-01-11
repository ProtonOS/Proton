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
				switch(curInstr.OpCode)
				{
					case LIROpCode.Move:
					{
						var curMove = (Instructions.Move)curInstr;
						if (curMove.Source.SourceType == SourceType.Local)
						{
							var loc = (LIRLocal)curMove.Source;
							if (loc.Dynamic && loc.AssignedValue != null && 
								(
									loc.AssignedValue.SourceType == SourceType.Local ||
									loc.AssignedValue.SourceType == SourceType.Literal
								)
							)
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
						break;
					}
					case LIROpCode.Math:
					{
						// Need to do dest
						var curMath = (Instructions.Math)curInstr;
						curMath.SourceA = ProcessSource(curMath.SourceA, method);
						curMath.SourceB = ProcessSource(curMath.SourceB, method);
						break;
					}
					case LIROpCode.Compare:
					{
						// Need to do dest
						var curCompare = (Instructions.Compare)curInstr;
						curCompare.SourceA = ProcessSource(curCompare.SourceA, method);
						curCompare.SourceB = ProcessSource(curCompare.SourceB, method);
						break;
					}
					case LIROpCode.BranchTrue:
					{
						var curBranch = (Instructions.BranchTrue)curInstr;
						curBranch.Source = ProcessSource(curBranch.Source, method);
						break;
					}
					case LIROpCode.Unary:
					{
						// Need to do dest
						var curUnary = (Instructions.Unary)curInstr;
						curUnary.Source = ProcessSource(curUnary.Source, method);
						break;
					}
					case LIROpCode.Return:
					{
						var curReturn = (Instructions.Return)curInstr;
						if (curReturn.Source != null)
							curReturn.Source = ProcessSource(curReturn.Source, method);
						break;
					}
					case LIROpCode.Call:
					{
						// Need to do dest
						var curCall = (Instructions.Call)curInstr;
						for (int i2 = 0; i2 < curCall.Sources.Count; i2++)
						{
							curCall.Sources[i2] = ProcessSource(curCall.Sources[i2], method);
						}
						break;
					}
					case LIROpCode.Convert:
					{
						// Need to do dest
						var curConvert = (Instructions.Convert)curInstr;
						curConvert.Source = ProcessSource(curConvert.Source, method);
						break;
					}

					case LIROpCode.Comment:
					case LIROpCode.Dead:
					case LIROpCode.Label:
					case LIROpCode.Nop:
					case LIROpCode.Branch:
						break;
					default:
						throw new Exception("Unknown LIROpCode!");
				}
				if (curInstr.MayHaveSideEffects)
					method.Locals.ForEach(l => { l.AssignedAt = -1; l.AssignedValue = null; });
			}
		}

		private static ISource ProcessSource(ISource src, LIRMethod method)
		{
			if (src.SourceType == SourceType.Local)
			{
				var loc = (LIRLocal)src;
				if (loc.Dynamic && loc.AssignedValue != null && loc.AssignedValue.SourceType == SourceType.Local)
				{
					method.mInstructions[loc.AssignedAt] = new Instructions.Dead(loc.AssignedAt);
					return loc.AssignedValue;
				}
			}
			return src;
		}
	}
}
