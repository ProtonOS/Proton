using System;
using System.Collections.Generic;

namespace Proton.LIR
{
	public sealed class LIRMethod
	{
		internal List<LIRInstruction> mInstructions = new List<LIRInstruction>();
		public IEnumerable<LIRInstruction> Instructions { get { return mInstructions; } }

		internal List<LIRLocal> mLocals = new List<LIRLocal>();
		public List<LIRLocal> Locals { get { return mLocals; } }

		internal List<LIRParameter> mParameters = new List<LIRParameter>();
		public List<LIRParameter> Parameters { get { return mParameters; } }

		public LIRLocal RequestLocal(LIRType tp)
		{
			return new LIRLocal(this, tp);
		}

		public void ReleaseLocal(LIRLocal l)
		{

		}
	}
}
