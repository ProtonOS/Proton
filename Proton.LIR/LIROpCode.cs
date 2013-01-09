using System;

namespace Proton.LIR
{
	public enum LIROpCode
	{
		Unknown = 0,
		Branch,
		BranchTrue,
		Call,
		Comment,
		Compare,
		Dead,
		Label,
		Math,
		Move,
		Nop,
		Return,
		Unary,
	}
}
