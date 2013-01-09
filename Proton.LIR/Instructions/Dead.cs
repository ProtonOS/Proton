using System;
using System.Collections.Generic;

namespace Proton.LIR.Instructions
{
	internal sealed class Dead : LIRInstruction
	{
		public Dead(int idx) : base(null, LIROpCode.Dead)
		{
			this.Index = idx;
		}

		internal override void Dump(IndentedStreamWriter wtr)
		{
			wtr.WriteLine("DEAD");
		}
	}
}
