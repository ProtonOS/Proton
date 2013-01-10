using System;
using System.Collections.Generic;

namespace Proton.LIR
{
	public class LIRLocal : ISource, IDestination
	{
		public bool MayHaveSideEffects { get { return false; } }
		public SourceType SourceType { get { return SourceType.Local; } }
		public DestinationType DestinationType { get { return DestinationType.Local; } }

		public LIRMethod Parent { get; private set; }

		public LIRType Type { get; private set; }
		public int Index { get; private set; }

		internal bool Dynamic { get; set; }
		internal ISource AssignedValue { get; set; }
		internal int AssignedAt { get; set; }

		public LIRLocal(LIRMethod parent, LIRType tp)
		{
			this.Parent = parent;
			this.Type = tp;

			this.Index = parent.mLocals.Count;
			this.Parent.mLocals.Add(this);
		}

		public ISource AddressOf()
		{
#warning Do me correctly....
			return this;
		}

		public override string ToString()
		{
			return String.Format("#{0}", Index);
		}

		public void Dump(IndentedStreamWriter wtr)
		{
			wtr.WriteLine("Local {0} {1}{2}", this, Type, (this.Dynamic ? " Dynamic" : ""));
		}
	}
}
