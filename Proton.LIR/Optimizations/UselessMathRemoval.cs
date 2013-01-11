using System;
using System.Collections.Generic;

namespace Proton.LIR.Optimizations
{
	internal sealed class LIRUselessMathRemovalOptimizationPass : LIROptimizationPass
	{
		public override void Run(LIRMethod method)
		{
			for (int i = 0; i < method.mInstructions.Count; i++)
			{
				var curInstr = method.mInstructions[i];
				switch (curInstr.OpCode)
				{
					case LIROpCode.Math:
					{
						var curMath = (Instructions.Math)curInstr;
						switch (curMath.Operation)
						{
							case Instructions.MathOperation.Add:
							case Instructions.MathOperation.Subtract:
								if (curMath.SourceA.SourceType == SourceType.Literal && ((LIRImm)curMath.SourceA).IsZero)
								{
									method.mInstructions[i] = new Instructions.Move(i, curMath.SourceB, curMath.Destination, curMath.ArgumentType);
								}
								else if (curMath.SourceB.SourceType == SourceType.Literal && ((LIRImm)curMath.SourceB).IsZero)
								{
									method.mInstructions[i] = new Instructions.Move(i, curMath.SourceA, curMath.Destination, curMath.ArgumentType);
								}
								break;
							case Instructions.MathOperation.Multiply:
								break;
							case Instructions.MathOperation.Divide:
								break;
							case Instructions.MathOperation.And:
								break;
							case Instructions.MathOperation.Or:
								break;
							case Instructions.MathOperation.Xor:
								break;
							case Instructions.MathOperation.ShiftLeft:
								break;
							case Instructions.MathOperation.ShiftRight:
								break;
							case Instructions.MathOperation.ShiftRightSignExtended:
								break;
							default:
								throw new Exception("Unknown MathOperation!");
						}
						break;
					}
					default:
						break;
				}
			}
		}
	}
}
