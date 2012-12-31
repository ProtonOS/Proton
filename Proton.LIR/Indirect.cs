using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Proton.LIR
{
	public class Indirect : ISource, IDestination
	{
		public ISource Source { get; private set; }

		public Indirect(ISource src)
		{
			this.Source = src;
		}
	}
}
