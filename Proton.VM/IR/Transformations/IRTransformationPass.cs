using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Transformations
{
	public abstract class IRTransformationPass
	{
		[Flags]
		public enum TransformType
		{
			Unknown = 0,
			Type,
			Method,
		}

		public abstract TransformType Type { get; }

		public virtual void Transform(IRMethod method)
		{
			throw new NotImplementedException("This pass doesn't implement a transform for methods!");
		}

		public virtual void Transform(IRType type)
		{
			throw new NotImplementedException("This pass doesn't implement a transform for types!");
		}
	}
}
