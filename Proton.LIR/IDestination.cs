using System;
using System.Collections.Generic;

namespace Proton.LIR
{
	public enum DestinationType
	{
		Local,
		Parameter,
		Indirect,
	}
	public interface IDestination
	{
		DestinationType DestinationType { get; }
		string ToString();
	}
}
