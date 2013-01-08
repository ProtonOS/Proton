using System;
using System.Collections.Generic;

namespace Proton.LIR.Instructions
{
	public sealed class BranchTrue : LIRInstruction
	{
		public ISource Source { get; private set; }
		public Label Target { get; private set; }

		public BranchTrue(LIRMethod parent, ISource source, Label target) : base(parent)
		{
			this.Source = source;
			this.Target = target;
		}

		internal override void Dump(IndentedStreamWriter wtr)
		{
			wtr.WriteLine("BranchTrue {0} -> {1}", Source, Target);
		}
	}
}
