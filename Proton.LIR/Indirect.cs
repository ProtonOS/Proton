﻿using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.LIR
{
	public class Indirect : ISource, IDestination
	{
		public bool MayHaveSideEffects { get { return true; } }
		public SourceType SourceType { get { return SourceType.Indirect; } }
		public DestinationType DestinationType { get { return DestinationType.Indirect; } }

		public ISource Source { get; private set; }

		public Indirect(ISource src)
		{
			this.Source = src;
		}

		public override string ToString()
		{
			return String.Format("[{0}]", Source);
		}
	}
}
