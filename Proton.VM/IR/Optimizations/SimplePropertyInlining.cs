using System;
using System.Collections.Generic;
using Proton.VM.IR.Instructions;

namespace Proton.VM.IR.Optimizations
{
	public sealed class IRSimplePropertyInliningOptimizationPass : IROptimizationPass
	{
		public override string Name { get { return "Simple Property Inlining"; } }
		public override string Description { get { return "Inlines the access of fields through simple properties."; } }
		public override IROptimizationPass.RunLocation Location { get { return RunLocation.DuringSSA; } }

		private static bool IsSimpleAccessor(IRMethod m, out int retInstrIdx)
		{
			// TODO: Allow for statics here as well.
			return
				(retInstrIdx = -1) == -1 &&
				m.ReturnType != null &&
				m.Parameters.Count == 1 &&
				m.Parameters[0].Type == m.ParentType &&
				(
					(
						m.Instructions.Count == 1 &&
						m.Instructions[0].Opcode == IROpcode.Return &&
						IsSimpleSource(m.Instructions[0].Sources[0]) &&
						(retInstrIdx = 0) == 0
					) ||
					(
						m.Instructions.Count == 2 &&
						m.Instructions[0].Opcode == IROpcode.Nop &&
						m.Instructions[1].Opcode == IROpcode.Return &&
						IsSimpleSource(m.Instructions[1].Sources[0]) &&
						(retInstrIdx = 1) == 1
					)
				)
			;	
		}

		private static bool IsSimpleSource(IRLinearizedLocation src)
		{
			switch (src.Type)
			{
				case IRLinearizedLocationType.Null:
				case IRLinearizedLocationType.String:
				case IRLinearizedLocationType.ConstantI4:
				case IRLinearizedLocationType.ConstantI8:
				case IRLinearizedLocationType.ConstantR4:
				case IRLinearizedLocationType.ConstantR8:
				case IRLinearizedLocationType.Field:
				case IRLinearizedLocationType.StaticField:
					return true;

				case IRLinearizedLocationType.Local:
				case IRLinearizedLocationType.LocalAddress:
				case IRLinearizedLocationType.Parameter:
				case IRLinearizedLocationType.ParameterAddress:
				case IRLinearizedLocationType.FieldAddress:
				case IRLinearizedLocationType.StaticFieldAddress:
				case IRLinearizedLocationType.Indirect:
				// TODO: May be able to inline array lengths as simple accessors at some point
				case IRLinearizedLocationType.ArrayLength:
				case IRLinearizedLocationType.ArrayElement:
				case IRLinearizedLocationType.ArrayElementAddress:
				case IRLinearizedLocationType.FunctionAddress:
				case IRLinearizedLocationType.RuntimeHandle:
				case IRLinearizedLocationType.Phi:
					return false;

				case IRLinearizedLocationType.SizeOf:
					throw new Exception("This should have already been eliminated in a transformation pass!");
				default:
					throw new Exception("Unknown IRLinearizedLocationType!");
			}
		}

		public override void Run(IRMethod pMethod)
		{
			pMethod.Instructions.ImmediateRetargetModifiedInstructions = false;
			for (int i = 0; i < pMethod.Instructions.Count; i++)
			{
				var curInstr = pMethod.Instructions[i];
				if (curInstr.Opcode == IROpcode.Call)
				{
					var curCall = (IRCallInstruction)curInstr;
					int retIdx;
					if (!curCall.Virtual && IsSimpleAccessor(curCall.Target, out retIdx))
					{
						if (curCall.Target.IsStatic)
						{
							var newMove = new IRMoveInstruction()
							{
								ParentMethod = pMethod,
							};
							newMove.Destination = curCall.Destination.Clone(newMove);
							var retSrc = curCall.Target.Instructions[retIdx].Sources[0];
							newMove.Sources.Add(retSrc.Clone(newMove));
							pMethod.Instructions[i] = newMove;
						}
						else
						{
							var newMove = new IRMoveInstruction()
							{
								ParentMethod = pMethod,
							};
							newMove.Destination = curCall.Destination.Clone(newMove);
							var curSrc = curCall.Sources[0];
							var retSrc = curCall.Target.Instructions[retIdx].Sources[0];
							var paramSrc = new IRLinearizedLocation(newMove, IRLinearizedLocationType.Parameter)
							{
								Parameter = new IRLinearizedLocation.ParameterLocationData()
								{
									ParameterIndex = 0,
								},
							};
							var nSrc = retSrc.Clone(newMove);
							nSrc.RetargetSource(ref nSrc, paramSrc, curSrc);
							newMove.Sources.Add(nSrc);
							pMethod.Instructions[i] = newMove;
						}
					}
				}
			}
			pMethod.Instructions.FixModifiedTargetInstructions();
			pMethod.Instructions.ImmediateRetargetModifiedInstructions = true;
		}
	}
}
