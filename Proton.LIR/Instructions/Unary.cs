using System;
using System.Collections.Generic;

namespace Proton.LIR.Instructions
{
	public enum UnaryOperation : byte
	{
		Negate,
		Not,
	}

	public sealed class Unary : LIRInstruction
	{
		internal override bool MayHaveSideEffects { get { return Source.MayHaveSideEffects || Destination.MayHaveSideEffects; } }
		public ISource Source { get; private set; }
		public IDestination Destination { get; private set; }
		public UnaryOperation Operation { get; private set; }
		public LIRType ArgumentType { get; private set; }

		public Unary(LIRMethod parent, ISource src, IDestination dest, UnaryOperation op, LIRType argType) : base(parent, LIROpCode.Unary)
		{
			Source = src;
			Destination = dest;
			Operation = op;
			ArgumentType = argType;
		}

		private static string GetOperationSymbol(UnaryOperation op)
		{
			switch (op)
			{
				case UnaryOperation.Negate:
					return "-";
				case UnaryOperation.Not:
					return "!";
				default:
					throw new NotSupportedException();
			}
		}
		internal override void Dump(IndentedStreamWriter wtr)
		{
			wtr.WriteLine("Unary {0} {1}{2} -> {3}", ArgumentType, GetOperationSymbol(Operation), Source, Destination);
		}
	}
}
