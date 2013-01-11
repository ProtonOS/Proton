using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Optimizations
{
	public abstract class IROptimizationPass
	{
		public enum RunLocation
		{
			First = 0,
			BeforeSSA,
			EnterSSA,
			DuringSSA,
			LeaveSSA,
			AfterSSA,
			Last,
		}

		public abstract string Name { get; }
		public abstract string Description { get; }

		public abstract RunLocation Location { get; }

		public abstract void Run(IRMethod pMethod);

	}
}
