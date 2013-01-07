using System;
using System.Collections.Generic;
using Proton.VM.IR.Instructions;

namespace Proton.VM.IR.Optimizations
{
	public sealed class IRNopKillingOptimizationPass : IROptimizationPass
	{
		public override string Name { get { return "Nop Killing"; } }
		public override string Description { get { return "Removes all Nop instructions that are set to not emit."; } }
		public override IROptimizationPass.RunLocation Location { get { return RunLocation.Last; } }

		public override void Run(IRMethod pMethod)
		{
			for (int i = 0; i < pMethod.Instructions.Count; i++)
			{
				var curInstr = pMethod.Instructions[i];
				if (curInstr.Opcode == IROpcode.Nop)
				{
					var nop = (IRNopInstruction)curInstr;
					if (!nop.ForceEmit)
					{
						pMethod.Instructions.Remove(nop);
						i--;
					}
				}
			}
			pMethod.Instructions.FixRemovedTargetInstructions();
		}
	}
}
