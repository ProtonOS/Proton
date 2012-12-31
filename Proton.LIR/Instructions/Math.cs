using System;
using System.Collections.Generic;

namespace Proton.LIR.Instructions
{
	public enum MathOperation : byte
	{
		Add,
		Subtract,
	}

	public class Math : LIRInstruction
	{
		public ISource SourceA { get; private set; }
		public ISource SourceB { get; private set; }
		public IDestination Destination { get; private set; }
		public MathOperation Operation { get; private set; }

		public Math(LIRMethod parent, ISource srcA, ISource srcB, IDestination dest, MathOperation op) : base(parent)
		{
			SourceA = srcA;
			SourceB = srcB;
			Destination = dest;
			Operation = op;
		}

		private static string GetOperationSymbol(MathOperation op)
		{
			switch (op)
			{
				case MathOperation.Add:
					return "+";
				case MathOperation.Subtract:
					return "-";
				default:
					throw new NotSupportedException();
			}
		}

		internal override void Dump(IndentedStreamWriter wtr)
		{
			wtr.WriteLine("Math {0} {1} {2} -> {3}", SourceA, GetOperationSymbol(Operation), SourceB, Destination);
		}

	}
}
