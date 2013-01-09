using System;
using System.Collections.Generic;

namespace Proton.LIR.Instructions
{
	public sealed class BranchTrue : LIRInstruction
	{
		internal override bool MayHaveSideEffects { get { return true; } }
		public ISource Source { get; internal set; }
		public Label Target { get; private set; }

		public BranchTrue(LIRMethod parent, ISource source, Label target) : base(parent, LIROpCode.BranchTrue)
		{
			this.Source = source;
			this.Target = target;
			target.References++;
		}

		internal override void Dump(IndentedStreamWriter wtr)
		{
			wtr.WriteLine("BranchTrue {0} -> {1}", Source, Target);
		}
	}
}
