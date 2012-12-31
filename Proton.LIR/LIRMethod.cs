using System;
using System.Collections.Generic;

namespace Proton.LIR
{
	public sealed class LIRMethod
	{
		internal List<LIRInstruction> mInstructions = new List<LIRInstruction>();
		public IEnumerable<LIRInstruction> Instructions { get { return mInstructions; } }

		internal List<LIRLocal> mLocals = new List<LIRLocal>();
		public IEnumerable<LIRLocal> Locals { get { return mLocals; } }

		internal List<LIRParameter> mParameters = new List<LIRParameter>();
		public IEnumerable<LIRParameter> Parameters { get { return mParameters; } }
	}
}
