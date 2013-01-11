using System;
using System.Collections.Generic;

namespace Proton.LIR
{
	public sealed class LIRParameter : ISource, IDestination
	{
		public bool MayHaveSideEffects { get { return false; } }
		public SourceType SourceType { get { return SourceType.Parameter; } }
		public DestinationType DestinationType { get { return DestinationType.Parameter; } }

		public LIRMethod Parent { get; private set; }

		public LIRType Type { get; private set; }
		public int Index { get; private set; }

		public LIRParameter(LIRMethod parent, LIRType tp)
		{
			this.Parent = parent;
			this.Type = tp;
			this.Index = parent.mParameters.Count;
			parent.mParameters.Add(this);
		}

		public ISource AddressOf()
		{
#warning Do me correctly....
			return this;
		}

		public void Dump(IndentedStreamWriter pWriter)
		{
			pWriter.Write(Type.ToString());
		}

		public override string ToString()
		{
			return String.Format("${0}", Index);
		}
	}
}
