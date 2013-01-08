using System;
using System.Collections.Generic;
using Proton.VM.IR.Instructions;

namespace Proton.VM.IR.Optimizations
{
	public sealed class IRCallDevirtualizationOptimizationPass : IROptimizationPass
	{
		public override string Name { get { return "Call Devirtualization"; } }
		public override string Description { get { return "Makes a call non-virtual if it can be proven that it will be made on an object of a specific type."; } }
		public override IROptimizationPass.RunLocation Location { get { return RunLocation.Last; } }

		private struct KnownLocalType
		{
			public IRType Type;
			public bool Known;
		}
		public override void Run(IRMethod pMethod)
		{
			KnownLocalType[] locals = new KnownLocalType[pMethod.Locals.Count];
			for (int i = 0; i < pMethod.Instructions.Count; i++)
			{
				var curInstr = pMethod.Instructions[i];
				if (curInstr.Opcode == IROpcode.NewObject && curInstr.Destination.Type == IRLinearizedLocationType.Local)
				{
					locals[curInstr.Destination.Local.LocalIndex].Known = true;
					locals[curInstr.Destination.Local.LocalIndex].Type = ((IRNewObjectInstruction)curInstr).Constructor.ParentType;
				}
			}
			for (int i = 0; i < pMethod.Instructions.Count; i++)
			{
				var curInstr = pMethod.Instructions[i];
				if (curInstr.Opcode == IROpcode.Call)
				{
					var callInstr = (IRCallInstruction)curInstr;
					if (callInstr.Virtual)
					{
						if (callInstr.Target.IsFinal || callInstr.Target.ParentType.IsSealed || callInstr.Sources[0].GetTypeOfLocation().IsSealed)
						{
							callInstr.Virtual = false;
						}
						else if (callInstr.Sources[0].Type == IRLinearizedLocationType.Local && locals[callInstr.Sources[0].Local.LocalIndex].Known)
						{
							callInstr.Target = locals[callInstr.Sources[0].Local.LocalIndex].Type.VirtualMethodTree[callInstr.Target.VirtualMethodIndex];
							callInstr.Virtual = false;
						}
					}
				}
			}


		}
	}
}
