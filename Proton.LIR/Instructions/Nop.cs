﻿using System;
using System.Collections.Generic;

namespace Proton.LIR.Instructions
{
	public sealed class Nop : LIRInstruction
	{
		public Nop(LIRMethod parent) : base(parent, LIROpCode.Nop)
		{
		}

		internal override void Dump(IndentedStreamWriter wtr)
		{
			wtr.WriteLine("Nop");
		}
	}
}
