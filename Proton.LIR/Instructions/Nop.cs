using System;
using System.Collections.Generic;

namespace Proton.LIR.Instructions
{
	public sealed class Nop : LIRInstruction
	{
		public Nop(LIRMethod parent) : base(parent)
		{
		}

		internal override void Dump(IndentedStreamWriter wtr)
		{
			wtr.WriteLine("Nop");
		}
	}
}
