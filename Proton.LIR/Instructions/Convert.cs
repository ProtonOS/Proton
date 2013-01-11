using System;
using System.Collections.Generic;

namespace Proton.LIR.Instructions
{
	public sealed class Convert : LIRInstruction
	{
		internal override bool MayHaveSideEffects { get { return Source.MayHaveSideEffects || Destination.MayHaveSideEffects; } }

		public ISource Source { get; internal set; }
		public LIRType SourceType { get; internal set; }
		public IDestination Destination { get; internal set; }
		public LIRType DestinationType { get; internal set; }

		public Convert(LIRMethod parent, ISource source, LIRType sourceType, IDestination dest, LIRType destType) : base(parent, LIROpCode.Convert)
		{
			Source = source;
			SourceType = sourceType;
			Destination = dest;
			DestinationType = destType;
		}

		internal override void Dump(IndentedStreamWriter wtr)
		{
			wtr.WriteLine("Convert {0} {1} -> {2} {3}", SourceType, Source, DestinationType, Destination);
		}
	}
}
