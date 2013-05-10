using System;
using System.Collections.Generic;

namespace Proton.LIR.Instructions
{
	public sealed class Branch : LIRInstruction
	{
		internal override bool MayHaveSideEffects { get { return true; } }
		public Label Target { get; private set; }

		public Branch(LIRMethod parent, Label target) : base(parent, LIROpCode.Branch)
		{
			this.Target = target;
			target.References++;
		}

		public Branch(LIRMethod parent)
			: base(parent, LIROpCode.Branch)
		{
		}

		public void SetTarget(Label target)
		{
			if (Target != null) Target.References--;
			this.Target = target;
			target.References++;
		}

		internal override void Dump(IndentedStreamWriter wtr)
		{
			wtr.WriteLine("Branch -> {0}", Target);
		}
	}
}
