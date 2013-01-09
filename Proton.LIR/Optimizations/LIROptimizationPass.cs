using System;
using System.Collections.Generic;

namespace Proton.LIR.Optimizations
{
	internal abstract class LIROptimizationPass
	{
		public abstract void Run(LIRMethod method);
	}
}
