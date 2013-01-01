using System;
using System.Collections.Generic;

namespace Proton.LIR
{
	public class LIRParameter : ISource, IDestination
	{
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

		public void Dump(IndentedStreamWriter pWriter)
		{
			pWriter.Write(Type.ToString());
		}

		public override string ToString()
		{
			return "$" + Index.ToString();
		}
	}
}
