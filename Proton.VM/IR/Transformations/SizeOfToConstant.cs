using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Transformations
{
	public sealed class IRSizeOfToConstantTransformationPass : IRTransformationPass
	{
		public override TransformType Type { get { return TransformType.Method; } }

		private static void TransformLocation(IRLinearizedLocation loc)
		{
			switch (loc.Type)
			{
				case IRLinearizedLocationType.Null: break;
				case IRLinearizedLocationType.ConstantI4: break;
				case IRLinearizedLocationType.ConstantI8: break;
				case IRLinearizedLocationType.ConstantR4: break;
				case IRLinearizedLocationType.ConstantR8: break;
				case IRLinearizedLocationType.String: break;
				case IRLinearizedLocationType.Parameter: break;
				case IRLinearizedLocationType.ParameterAddress: break;
				case IRLinearizedLocationType.StaticField: break;
				case IRLinearizedLocationType.StaticFieldAddress: break;
				case IRLinearizedLocationType.RuntimeHandle: break;
				case IRLinearizedLocationType.Local: break;
				case IRLinearizedLocationType.LocalAddress: break;
				case IRLinearizedLocationType.SizeOf:
					loc.Type = IRLinearizedLocationType.ConstantI4;
					loc.ConstantI4.Value = loc.SizeOf.Type.StackSize;
					loc.SizeOf.Type = null;
					break;
				case IRLinearizedLocationType.Field:
					TransformLocation(loc.Field.FieldLocation);
					break;
				case IRLinearizedLocationType.FieldAddress:
					TransformLocation(loc.FieldAddress.FieldLocation);
					break;
				case IRLinearizedLocationType.Indirect:
					TransformLocation(loc.Indirect.AddressLocation);
					break;
				case IRLinearizedLocationType.ArrayElement:
					TransformLocation(loc.ArrayElement.ArrayLocation);
					TransformLocation(loc.ArrayElement.IndexLocation);
					break;
				case IRLinearizedLocationType.ArrayElementAddress:
					TransformLocation(loc.ArrayElementAddress.ArrayLocation);
					TransformLocation(loc.ArrayElementAddress.IndexLocation);
					break;
				case IRLinearizedLocationType.ArrayLength:
					TransformLocation(loc.ArrayLength.ArrayLocation);
					break;
				case IRLinearizedLocationType.FunctionAddress:
					if (loc.FunctionAddress.Virtual)
						TransformLocation(loc.FunctionAddress.InstanceLocation);
					break;
				case IRLinearizedLocationType.Phi:
					throw new Exception("Phi's shouldn't exist yet!");
				default:
					throw new Exception("Unknown IRLinearizedLocation type!");
			}
		}

		public override void Transform(IRMethod method)
		{
			foreach (var instr in method.Instructions)
			{
				instr.Sources.ForEach(s => TransformLocation(s));
				if (instr.Destination != null)
					TransformLocation(instr.Destination);
			}
		}
	}
}
