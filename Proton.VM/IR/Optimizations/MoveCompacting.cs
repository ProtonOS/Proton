using System;
using System.Collections.Generic;
using Proton.VM.IR.Instructions;

namespace Proton.VM.IR.Optimizations
{
	public sealed class IRMoveCompactingOptimizationPass : IROptimizationPass
	{
		public override string Name { get { return "Move Compacting"; } }
		public override string Description { get { return "Compacts a move to a move into a single move."; } }
		public override IROptimizationPass.RunLocation Location { get { return RunLocation.DuringSSA; } }

		private sealed class AssignedLocal
		{
			public int AssignedAt = -1;
			public bool Killed;
			public bool AddressLoaded;
			public int UseCount;

			public override string ToString()
			{
				return (Killed ? "DEAD " : "") + UseCount + " @ " + AssignedAt + (AddressLoaded ? " AddressLoaded" : "");
			}
		}

		private static void ProcessLocation(IRLinearizedLocation s, AssignedLocal[] locals)
		{
			switch (s.Type)
			{
				case IRLinearizedLocationType.Null: break;
				case IRLinearizedLocationType.ConstantI4: break;
				case IRLinearizedLocationType.ConstantI8: break;
				case IRLinearizedLocationType.ConstantR4: break;
				case IRLinearizedLocationType.ConstantR8: break;
				case IRLinearizedLocationType.SizeOf: break;
				case IRLinearizedLocationType.String: break;
				case IRLinearizedLocationType.Parameter: break;
				case IRLinearizedLocationType.ParameterAddress: break;
				case IRLinearizedLocationType.StaticField: break;
				case IRLinearizedLocationType.StaticFieldAddress: break;
				case IRLinearizedLocationType.RuntimeHandle: break;
				case IRLinearizedLocationType.Local:
					locals[s.Local.LocalIndex].UseCount++;
					break;
				case IRLinearizedLocationType.LocalAddress:
					locals[s.LocalAddress.LocalIndex].UseCount++;
					locals[s.LocalAddress.LocalIndex].AddressLoaded = true;
					break;
				case IRLinearizedLocationType.Field:
					ProcessLocation(s.Field.FieldLocation, locals);
					break;
				case IRLinearizedLocationType.FieldAddress:
					ProcessLocation(s.FieldAddress.FieldLocation, locals);
					break;
				case IRLinearizedLocationType.Indirect:
					ProcessLocation(s.Indirect.AddressLocation, locals);
					break;
				case IRLinearizedLocationType.ArrayElement:
					ProcessLocation(s.ArrayElement.ArrayLocation, locals);
					ProcessLocation(s.ArrayElement.IndexLocation, locals);
					break;
				case IRLinearizedLocationType.ArrayElementAddress:
					ProcessLocation(s.ArrayElementAddress.ArrayLocation, locals);
					ProcessLocation(s.ArrayElementAddress.IndexLocation, locals);
					break;
				case IRLinearizedLocationType.ArrayLength:
					ProcessLocation(s.ArrayLength.ArrayLocation, locals);
					break;
				case IRLinearizedLocationType.FunctionAddress:
					if (s.FunctionAddress.Virtual)
						ProcessLocation(s.FunctionAddress.InstanceLocation, locals);
					break;
				case IRLinearizedLocationType.Phi:
					s.Phi.SourceLocations.ForEach(source => ProcessLocation(source, locals));
					break;
				default:
					throw new Exception("Unknown IRLinearizedLocation type!");
			}
		}

		private static void RetargetLocation(IRMethod pMethod, ref IRLinearizedLocation location, AssignedLocal[] locals)
		{
			switch (location.Type)
			{
				case IRLinearizedLocationType.Null: break;
				case IRLinearizedLocationType.ConstantI4: break;
				case IRLinearizedLocationType.ConstantI8: break;
				case IRLinearizedLocationType.ConstantR4: break;
				case IRLinearizedLocationType.ConstantR8: break;
				case IRLinearizedLocationType.SizeOf: break;
				case IRLinearizedLocationType.String: break;
				case IRLinearizedLocationType.Parameter: break;
				case IRLinearizedLocationType.ParameterAddress: break;
				case IRLinearizedLocationType.StaticField: break;
				case IRLinearizedLocationType.StaticFieldAddress: break;
				case IRLinearizedLocationType.RuntimeHandle: break;
				case IRLinearizedLocationType.LocalAddress: break;

				case IRLinearizedLocationType.Local:
					var l = locals[location.Local.LocalIndex];
					if (l.UseCount == 1 && l.AssignedAt != -1 && !l.AddressLoaded && !pMethod.Locals[location.Local.LocalIndex].SSAData.Phi)
					{
						location = pMethod.Instructions[l.AssignedAt].Sources[0].Clone(location.ParentInstruction);
						l.Killed = true;
					}
					break;
				case IRLinearizedLocationType.Field:
					RetargetLocation(pMethod, ref location.Field.FieldLocation, locals);
					break;
				case IRLinearizedLocationType.FieldAddress:
					RetargetLocation(pMethod, ref location.FieldAddress.FieldLocation, locals);
					break;
				case IRLinearizedLocationType.Indirect:
					RetargetLocation(pMethod, ref location.Indirect.AddressLocation, locals);
					break;
				case IRLinearizedLocationType.ArrayElement:
					RetargetLocation(pMethod, ref location.ArrayElement.ArrayLocation, locals);
					RetargetLocation(pMethod, ref location.ArrayElement.IndexLocation, locals);
					break;
				case IRLinearizedLocationType.ArrayElementAddress:
					RetargetLocation(pMethod, ref location.ArrayElementAddress.ArrayLocation, locals);
					RetargetLocation(pMethod, ref location.ArrayElementAddress.IndexLocation, locals);
					break;
				case IRLinearizedLocationType.ArrayLength:
					RetargetLocation(pMethod, ref location.ArrayLength.ArrayLocation, locals);
					break;
				case IRLinearizedLocationType.FunctionAddress:
					if (location.FunctionAddress.Virtual)
						RetargetLocation(pMethod, ref location.FunctionAddress.InstanceLocation, locals);
					break;
				case IRLinearizedLocationType.Phi:
					for (int i = 0; i < location.Phi.SourceLocations.Count; i++)
					{
						var s = location.Phi.SourceLocations[i];
						RetargetLocation(pMethod, ref s, locals);
						location.Phi.SourceLocations[i] = s;
					}
					break;
				default:
					throw new Exception("Unknown IRLinearizedLocation type!");
			}
		}

		public override void Run(IRMethod pMethod)
		{
			pMethod.Instructions.ImmediateRetargetModifiedInstructions = false;

			var localUseMap = new AssignedLocal[pMethod.Locals.Count];
			for (int i = 0; i < localUseMap.Length; i++) localUseMap[i] = new AssignedLocal();

			for (int i = 0; i < pMethod.Instructions.Count; i++)
			{
				var curInstr = pMethod.Instructions[i];
				curInstr.Sources.ForEach(s => ProcessLocation(s, localUseMap));
				if (curInstr.Destination != null)
				{
					ProcessLocation(curInstr.Destination, localUseMap);
					if (curInstr.Opcode == IROpcode.Move && curInstr.Destination.Type == IRLinearizedLocationType.Local)
					{
						if (localUseMap[curInstr.Destination.Local.LocalIndex].AssignedAt >= 0)
							throw new Exception("Somehow it was already assigned!");
						localUseMap[curInstr.Destination.Local.LocalIndex].UseCount--;
						localUseMap[curInstr.Destination.Local.LocalIndex].AssignedAt = i;
					}
				}
			}

			for (int i = 0; i < pMethod.Instructions.Count; i++)
			{
				var curInstr = pMethod.Instructions[i];
				for (int i2 = 0; i2 < curInstr.Sources.Count; i2++)
				{
					var s = curInstr.Sources[i2];
					RetargetLocation(pMethod, ref s, localUseMap);
					curInstr.Sources[i2] = s;
				}
				if (curInstr.Destination != null)
					RetargetLocation(pMethod, ref curInstr.Destination, localUseMap);
			}

			for (int i = 0; i < localUseMap.Length; i++)
			{
				var l = localUseMap[i];
				if (l.Killed)
					pMethod.Instructions[l.AssignedAt] = new IRNopInstruction();
			}

			pMethod.Instructions.FixModifiedTargetInstructions();
			pMethod.Instructions.ImmediateRetargetModifiedInstructions = true;
		}
	}
}
