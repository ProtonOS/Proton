using System;
using System.Collections.Generic;

namespace Proton.LIR
{
	public sealed class LIRParameterAddress : ISource
	{
		public bool MayHaveSideEffects { get { return true; } }
		public SourceType SourceType { get { return SourceType.ParameterAddress; } }

		public LIRParameter Parent { get; private set; }

		internal LIRParameterAddress(LIRParameter parent)
		{
			this.Parent = parent;
		}

		public override string ToString()
		{
			return String.Format("&{0}", Parent);
		}
	}

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

		private LIRParameterAddress mAddressCache = null;
		public LIRParameterAddress AddressOf() { return mAddressCache ?? (mAddressCache = new LIRParameterAddress(this)); }

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
