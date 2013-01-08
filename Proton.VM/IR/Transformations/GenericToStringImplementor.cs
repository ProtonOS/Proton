using System;
using System.Collections.Generic;
using Proton.VM.IR.Instructions;

namespace Proton.VM.IR.Transformations
{
	public sealed class IRGenericToStringImplementorTransformationPass : IRTransformationPass
	{
		public override TransformType Type { get { return TransformType.Type; } }

		public override void Transform(IRType type)
		{
			if (!type.IsAbstract && (type.IsGeneric || type.NestedInsideOfType != null || type.IsArrayType || type.IsManagedPointerType || type.IsUnmanagedPointerType))
			{
				if (type.VirtualMethodTree[3].ParentType != type)
				{
					// It doesn't already implement ToString() itself.
					IRMethod ts = new IRMethod(type.Assembly);
					ts.ParentType = type;
					ts.ReturnType = type.Assembly.AppDomain.System_String;
					ts.Parameters.Add(new IRParameter(type.Assembly) { ParentMethod = ts, Type = type });
					ts.VirtualMethodIndex = 3;
					var r = new IRReturnInstruction();
					r.ParentMethod = ts;
					r.Linearized = true;
					var loc = new IRLinearizedLocation(r, IRLinearizedLocationType.String);
					loc.String.Value = type.ToString();
					r.Sources.Add(loc);
					ts.Instructions.Add(r);

					ts.ParentTypeMethodIndex = type.Methods.Count;
					type.Methods.Add(ts);
					type.VirtualMethodTree[3] = ts;

					if (!ts.Resolved) // This adds it to the Domain's Methods list.
						throw new Exception();
				}
			}
		}
	}
}
