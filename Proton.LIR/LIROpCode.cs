using System;

namespace Proton.LIR
{
	public enum LIROpCode
	{
		Unknown = 0,
		Branch,
		BranchIndirect,
		BranchTrue,
		Call,
		CallIndirect,
		Comment,
		Compare,
		Convert,
		Dead,
		Label,
		Math,
		Move,
		Nop,
		Return,
		Unary,
	}
}
