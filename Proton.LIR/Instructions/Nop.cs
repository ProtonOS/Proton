using System;
using System.Collections.Generic;

namespace Proton.LIR.Instructions
{
	public sealed class Nop : LIRInstruction
	{
		internal override bool MayHaveSideEffects { get { return false; } }
		public Nop(LIRMethod parent) : base(parent, LIROpCode.Nop)
		{
		}

		internal override void Dump(IndentedStreamWriter wtr)
		{
			wtr.WriteLine("Nop");
		}
	}
}
