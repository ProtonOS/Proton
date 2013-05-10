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
		Modulus,

		And,
		Or,
		Xor,

		ShiftLeft,
		ShiftRight,
		ShiftRightSignExtended,
	}

	public sealed class Math : LIRInstruction
	{
		internal override bool MayHaveSideEffects { get { return SourceA.MayHaveSideEffects || SourceB.MayHaveSideEffects || Destination.MayHaveSideEffects; } }
		public ISource SourceA { get; internal set; }
		public ISource SourceB { get; internal set; }
		public IDestination Destination { get; internal set; }
		public MathOperation Operation { get; private set; }
		public LIRType ArgumentType { get; private set; }

		public Math(LIRMethod parent, ISource srcA, ISource srcB, IDestination dest, MathOperation op, LIRType argType) : base(parent, LIROpCode.Math)
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
				case MathOperation.Modulus:
					return "%";
				case MathOperation.And:
					return "&";
				case MathOperation.Or:
					return "|";
				case MathOperation.Xor:
					return "^";
				case MathOperation.ShiftLeft:
					return "<<";
				case MathOperation.ShiftRight:
					return ">>";
				case MathOperation.ShiftRightSignExtended:
					return ">>>";
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
