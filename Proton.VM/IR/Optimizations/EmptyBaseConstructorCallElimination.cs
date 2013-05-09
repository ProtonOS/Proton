using System;
using System.Collections.Generic;
using Proton.VM.IR.Instructions;

namespace Proton.VM.IR.Optimizations
{
	public sealed class IREmptyBaseConstructorCallEliminationOptimizationPass : IROptimizationPass
	{
		public override string Name { get { return "Empty Base Constructor Call Elimination Removal"; } }
		public override string Description { get { return "Removes calls to base constructors that are empty."; } }
		public override IROptimizationPass.RunLocation Location { get { return RunLocation.DuringSSA; } }

		private static bool IsEmptyConstructor(IRMethod m)
		{
			return 
				(
					m.Instructions.Count == 3 &&
					m.Instructions[0].Opcode == IROpcode.Call &&
					IsBaseCallInstruction(m, m.Instructions[0]) &&
					IsEmptyConstructor(((IRCallInstruction)m.Instructions[0]).Target) &&
					m.Instructions[1].Opcode == IROpcode.Nop &&
					m.Instructions[2].Opcode == IROpcode.Return
				) ||
				(
					m.Instructions.Count == 2 &&
					(
						m.Instructions[0].Opcode == IROpcode.Nop ||
						(
							m.Instructions[0].Opcode == IROpcode.Call &&
							IsBaseCallInstruction(m, m.Instructions[0]) &&
							IsEmptyConstructor(((IRCallInstruction)m.Instructions[0]).Target)
						)
					) &&
					m.Instructions[1].Opcode == IROpcode.Return
				) ||
				(
					m.Instructions.Count == 1 &&
					m.Instructions[0].Opcode == IROpcode.Return
				)
			;
		}

		private static bool IsBaseCallInstruction(IRMethod pMethod, IRInstruction instr)
		{
			if (((IRCallInstruction)instr).Target.ParentType != pMethod.ParentType.BaseType)
			{
				int i43 = 0;
				i43++;
			}
			return
				instr.Sources.Count == 1 &&
				instr.Sources[0].Type == IRLinearizedLocationType.Parameter &&
				instr.Sources[0].Parameter.ParameterIndex == 0 &&
				((IRCallInstruction)instr).Target.ParentType == pMethod.ParentType.BaseType
			;
		}

		public override void Run(IRMethod pMethod)
		{
			if (
				pMethod.Name == ".ctor" && 
				pMethod.Instructions.Count >= 3 &&
				pMethod.ParentType.BaseType != null
			)
			{
				int c = pMethod.Instructions.Count;
				int cIdx = -1;
				pMethod.Instructions.ImmediateRetargetModifiedInstructions = false;
				if (
					(
						pMethod.Instructions[c - 3].Opcode == IROpcode.Call &&
						IsBaseCallInstruction(pMethod, pMethod.Instructions[c - 3]) &&
						(cIdx = c - 3) >= 0
					) ||
					(
						pMethod.Instructions[0].Opcode == IROpcode.Call &&
						IsBaseCallInstruction(pMethod, pMethod.Instructions[0]) &&
						(cIdx = 0) >= 0
					) ||
					(
						pMethod.Instructions[c - 2].Opcode == IROpcode.Call &&
						IsBaseCallInstruction(pMethod, pMethod.Instructions[c - 2]) &&
						(cIdx = c - 2) >= 0
					)
				)
				{
					if (IsEmptyConstructor(((IRCallInstruction)pMethod.Instructions[cIdx]).Target))
					{
						pMethod.Instructions[cIdx] = new IRNopInstruction();
					}
				}
				pMethod.Instructions.FixModifiedTargetInstructions();
				pMethod.Instructions.ImmediateRetargetModifiedInstructions = true;
			}
		}
	}
}
