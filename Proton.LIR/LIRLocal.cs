using System;
using System.Collections.Generic;

namespace Proton.LIR
{
	public class LIRLocal : ISource, IDestination
	{
		public LIRMethod Parent { get; private set; }

		public LIRType Type { get; private set; }
		public int Index { get; private set; }

		public LIRLocal(LIRMethod parent, LIRType tp)
		{
			this.Parent = parent;
			this.Type = tp;

			this.Index = parent.mLocals.Count;
			this.Parent.mLocals.Add(this);
		}

		public override string ToString()
		{
			return "#" + Index.ToString();
		}
	}
}
