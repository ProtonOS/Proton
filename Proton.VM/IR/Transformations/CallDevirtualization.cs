using System;
using System.Collections.Generic;
using Proton.VM.IR.Instructions;

namespace Proton.VM.IR.Transformations
{
	public sealed class IRCallDevirtualizationTransformationPass : IRTransformationPass
	{
		public override TransformType Type { get { return TransformType.Method; } }

		private struct KnownLocalType
		{
			public IRType Type;
			public bool Known;
		}
		public override void Transform(IRMethod pMethod)
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
						if (!callInstr.Target.IsVirtual || callInstr.Target.IsFinal || callInstr.Target.ParentType.IsSealed || callInstr.Sources[0].GetTypeOfLocation().IsSealed)
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
