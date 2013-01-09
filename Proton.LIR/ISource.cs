using System;
using System.Collections.Generic;

namespace Proton.LIR
{
	public enum SourceType
	{
		Local,
		LocalAddress,
		Parameter,
		ParameterAddress,
		Indirect,
		Literal,
	}
	public interface ISource
	{
		bool MayHaveSideEffects { get; }
		SourceType SourceType { get; }
		string ToString();
	}
}
