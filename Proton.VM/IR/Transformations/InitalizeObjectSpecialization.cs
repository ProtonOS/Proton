using System;
using System.Collections.Generic;
using Proton.VM.IR.Instructions;

namespace Proton.VM.IR.Transformations
{
	public sealed class IRInitalizeObjectSpecializationTransformationPass : IRTransformationPass
	{
		public override TransformType Type { get { return TransformType.Method; } }

		public override void Transform(IRMethod method)
		{
			method.Instructions.ImmediateRetargetModifiedInstructions = false;
			for (int i = 0; i < method.Instructions.Count; i++)
			{
				var curInstr = method.Instructions[i];
				if (curInstr.Opcode == IROpcode.InitializeObject)
				{
					var initObj = (IRInitializeObjectInstruction)curInstr;
					var mov = new IRMoveInstruction()
					{
						ParentMethod = method,
						Linearized = true,
						IRIndex = i,
						ILOffset = curInstr.ILOffset,
					};
					if (initObj.Type.IsClass || initObj.Type.IsInterface)
					{
						mov.Destination = new IRLinearizedLocation(mov, IRLinearizedLocationType.Indirect)
						{
							Indirect = new IRLinearizedLocation.IndirectLocationData()
							{
								AddressLocation = initObj.Sources[0],
								Type = method.Assembly.AppDomain.System_Object
							}
						};
						mov.Sources.Add(new IRLinearizedLocation(mov, IRLinearizedLocationType.Null));
					}
					else if (
						initObj.Type == method.Assembly.AppDomain.System_Byte ||
						initObj.Type == method.Assembly.AppDomain.System_SByte ||
						initObj.Type == method.Assembly.AppDomain.System_UInt16 ||
						initObj.Type == method.Assembly.AppDomain.System_Int16 ||
						initObj.Type == method.Assembly.AppDomain.System_Char ||
						initObj.Type == method.Assembly.AppDomain.System_UInt32 ||
						initObj.Type == method.Assembly.AppDomain.System_Int32
					)
					{
						mov.Destination = new IRLinearizedLocation(mov, IRLinearizedLocationType.Indirect)
						{
							Indirect = new IRLinearizedLocation.IndirectLocationData()
							{
								AddressLocation = initObj.Sources[0],
								Type = initObj.Type
							}
						};
						mov.Sources.Add(new IRLinearizedLocation(mov, IRLinearizedLocationType.ConstantI4));
					}
					else if (
						initObj.Type == method.Assembly.AppDomain.System_UInt64 ||
						initObj.Type == method.Assembly.AppDomain.System_Int64
					)
					{
						mov.Destination = new IRLinearizedLocation(mov, IRLinearizedLocationType.Indirect)
						{
							Indirect = new IRLinearizedLocation.IndirectLocationData()
							{
								AddressLocation = initObj.Sources[0],
								Type = initObj.Type
							}
						};
						mov.Sources.Add(new IRLinearizedLocation(mov, IRLinearizedLocationType.ConstantI8));
					}
					else if (initObj.Type == method.Assembly.AppDomain.System_Single)
					{
						mov.Destination = new IRLinearizedLocation(mov, IRLinearizedLocationType.Indirect)
						{
							Indirect = new IRLinearizedLocation.IndirectLocationData()
							{
								AddressLocation = initObj.Sources[0],
								Type = initObj.Type
							}
						};
						mov.Sources.Add(new IRLinearizedLocation(mov, IRLinearizedLocationType.ConstantR4));
					}
					else if (initObj.Type == method.Assembly.AppDomain.System_Double)
					{
						mov.Destination = new IRLinearizedLocation(mov, IRLinearizedLocationType.Indirect)
						{
							Indirect = new IRLinearizedLocation.IndirectLocationData()
							{
								AddressLocation = initObj.Sources[0],
								Type = initObj.Type
							}
						};
						mov.Sources.Add(new IRLinearizedLocation(mov, IRLinearizedLocationType.ConstantR8));
					}
					else
					{
#warning Still need a way to initialize a structure here.
						continue;
					}
					method.Instructions[i] = mov;
				}
			}
			method.Instructions.FixModifiedTargetInstructions();
			method.Instructions.ImmediateRetargetModifiedInstructions = true;
		}
	}
}
