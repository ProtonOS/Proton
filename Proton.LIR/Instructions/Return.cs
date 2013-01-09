using System;
using System.Collections.Generic;

namespace Proton.LIR.Instructions
{
	public sealed class Return : LIRInstruction
	{
		internal override bool MayHaveSideEffects { get { return true; } }
		public ISource Source { get; internal set; }
		public LIRType SourceType { get; private set; }

		public Return(LIRMethod parent, ISource source = null, LIRType sourceType = null) : base(parent, LIROpCode.Return)
		{
			this.Source = source;
			this.SourceType = sourceType;
		}

		internal override void Dump(IndentedStreamWriter wtr)
		{
			wtr.Write("Return");
			if (Source != null)
				wtr.Write(" {0} {1}", SourceType, Source);
			wtr.WriteLine();
		}
	}
}
