using System;
using System.Collections.Generic;

namespace Proton.LIR
{
	public abstract class LIRInstruction
	{
		internal abstract bool MayHaveSideEffects { get; }
		public int Index { get; internal set; }
		public LIROpCode OpCode { get; private set; }

		protected LIRInstruction(LIRMethod parent, LIROpCode opCode)
		{
			this.OpCode = opCode;
			if (parent != null)
			{
				this.Index = parent.mInstructions.Count;
				parent.mInstructions.Add(this);
			}
		}

		internal abstract void Dump(IndentedStreamWriter wtr);
	}
}
