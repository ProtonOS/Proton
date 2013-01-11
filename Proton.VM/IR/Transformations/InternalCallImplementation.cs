using System;
using System.Collections.Generic;
using Proton.VM.IR.Instructions;

namespace Proton.VM.IR.Transformations
{
	public sealed class IRInternalCallImplementationTransformationPass : IRTransformationPass
	{
		public override TransformType Type { get { return TransformType.Method; } }

		public override void Transform(IRMethod method)
		{
			if (method.IsInternalCall)
			{
				IRInstruction added = null;
				switch (method.ToString())
				{
					case "object object::Internal_PointerToReference(void*)":
					case "void* object.Internal_ReferenceToPointer(object)":
						method.Instructions.Add(
							added = new IRReturnInstruction()
							{
								Sources = new List<IRLinearizedLocation>()
								{
									new IRLinearizedLocation(null, IRLinearizedLocationType.Parameter)
									{
										Parameter = new IRLinearizedLocation.ParameterLocationData()
										{
											ParameterIndex = 0
										}
									}
								}
							}
						);
						added.Sources[0].SetParentInstruction(added);
						added.ParentMethod = method;
						break;
					case "(null) object::Internal_FastCopy(void*, void*, int)":
						break;
					case "(null) object::Internal_FastZero(void*, int)":
						break;
					case "(null) string..ctor(string, char[])":
						break;
					case "(null) string..ctor(string, char, int)":
						break;
					default:
						throw new Exception("Unknown internal call '" + method.ToString() + "'!");
				}
			}
		}
	}
}
