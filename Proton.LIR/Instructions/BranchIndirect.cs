using System;
using System.Collections.Generic;

namespace Proton.LIR.Instructions
{
	public sealed class BranchIndirect : LIRInstruction
	{
		internal override bool MayHaveSideEffects { get { return true; } }
		public ISource Source { get; internal set; }

		public BranchIndirect(LIRMethod parent, ISource source) : base(parent, LIROpCode.BranchIndirect)
		{
			this.Source = source;
		}

		internal override void Dump(IndentedStreamWriter wtr)
		{
			wtr.WriteLine("BranchIndirect [{0}]", Source);
		}
	}
}
