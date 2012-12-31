using System;
using System.Collections.Generic;

namespace Proton.LIR
{
	public abstract class LIRInstruction
	{
		public int Index { get; private set; }

		protected LIRInstruction(LIRMethod parent)
		{
			this.Index = parent.mInstructions.Count;
			parent.mInstructions.Add(this);
		}

		internal abstract void Dump(IndentedStreamWriter wtr);
	}
}
