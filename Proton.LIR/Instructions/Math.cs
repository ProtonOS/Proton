using System;
using System.Collections.Generic;

namespace Proton.LIR.Instructions
{
	public enum MathOperation : byte
	{
		Add,
		Subtract,
		Multiply,
		Divide,

		And,
		Or,
		Xor,
	}

	public sealed class Math : LIRInstruction
	{
		public ISource SourceA { get; private set; }
		public ISource SourceB { get; private set; }
		public IDestination Destination { get; private set; }
		public MathOperation Operation { get; private set; }
		public LIRType ArgumentType { get; private set; }

		public Math(LIRMethod parent, ISource srcA, ISource srcB, IDestination dest, MathOperation op, LIRType argType) : base(parent)
		{
			SourceA = srcA;
			SourceB = srcB;
			Destination = dest;
			Operation = op;
			ArgumentType = argType;
		}

		private static string GetOperationSymbol(MathOperation op)
		{
			switch (op)
			{
				case MathOperation.Add:
					return "+";
				case MathOperation.Subtract:
					return "-";
				case MathOperation.Multiply:
					return "*";
				case MathOperation.Divide:
					return "/";
				case MathOperation.And:
					return "&";
				case MathOperation.Or:
					return "|";
				case MathOperation.Xor:
					return "^";
				default:
					throw new NotSupportedException();
			}
		}

		internal override void Dump(IndentedStreamWriter wtr)
		{
			wtr.WriteLine("Math {0} {1} {2} {3} -> {4}", ArgumentType, SourceA, GetOperationSymbol(Operation), SourceB, Destination);
		}

	}
}
