using System;
using System.Collections.Generic;

namespace Proton.LIR.Instructions
{
	public sealed class Move : LIRInstruction
	{
		internal override bool MayHaveSideEffects { get { return Source.MayHaveSideEffects || Destination.MayHaveSideEffects; } }
		public ISource Source { get; internal set; }
		public IDestination Destination { get; internal set; }
		public LIRType ArgumentType { get; private set; }

		public Move(LIRMethod parent, ISource src, IDestination dest, LIRType argType) : base(parent, LIROpCode.Move)
		{
			Source = src;
			Destination = dest;
			ArgumentType = argType;
		}

		internal Move(int idx, ISource src, IDestination dest, LIRType argType) : base(idx, LIROpCode.Move)
		{
			Source = src;
			Destination = dest;
			ArgumentType = argType;
		}

		internal override void Dump(IndentedStreamWriter wtr)
		{
			wtr.WriteLine("Move {0} {1} -> {2}", ArgumentType, Source, Destination);
		}
	}
}
