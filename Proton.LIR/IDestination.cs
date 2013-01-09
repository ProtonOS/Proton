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
		bool MayHaveSideEffects { get; }
		DestinationType DestinationType { get; }
		string ToString();
	}
}
