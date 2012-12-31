using System;
using System.Collections.Generic;

namespace Proton.LIR.Instructions
{
	public class Move : LIRInstruction
	{
		public ISource Source { get; private set; }
		public IDestination Destination { get; private set; }

		public Move(LIRMethod parent, ISource src, IDestination dest) : base(parent)
		{
			Source = src;
			Destination = dest;
		}

		internal override void Dump(IndentedStreamWriter wtr)
		{
			wtr.WriteLine("Move {0} -> {1}", Source, Destination);
		}
	}
}
