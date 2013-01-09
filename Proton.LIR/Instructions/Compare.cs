using System;
using System.Collections.Generic;

namespace Proton.LIR.Instructions
{
	public enum CompareCondition
	{
		Equal,
		NotEqual,
		GreaterThan,
		GreaterThanOrEqual,
		LessThan,
		LessThanOrEqual,
	}

	public sealed class Compare : LIRInstruction
	{
		internal override bool MayHaveSideEffects { get { return SourceA.MayHaveSideEffects || SourceB.MayHaveSideEffects || Destination.MayHaveSideEffects; } }
		public ISource SourceA { get; private set; }
		public ISource SourceB { get; private set; }
		public IDestination Destination { get; private set; }
		public LIRType Type { get; private set; }
		public CompareCondition Condition { get; private set; }

		public Compare(LIRMethod parent, ISource sourceA, ISource sourceB, IDestination destination, LIRType type, CompareCondition condition) : base(parent, LIROpCode.Compare)
		{
			this.SourceA = sourceA;
			this.SourceB = sourceB;
			this.Destination = destination;
			this.Type = type;
			this.Condition = condition;
		}

		private static string GetConditionSymbol(CompareCondition c)
		{
			switch (c)
			{
				case CompareCondition.Equal: return "==";
				case CompareCondition.NotEqual: return "!=";
				case CompareCondition.GreaterThan: return ">";
				case CompareCondition.GreaterThanOrEqual: return ">=";
				case CompareCondition.LessThan: return "<";
				case CompareCondition.LessThanOrEqual: return "<=";
				default: throw new Exception("Unknown CompareCondition!");
			}
		}

		internal override void Dump(IndentedStreamWriter wtr)
		{
			wtr.WriteLine("Compare {0} {1} {2} -> {3}", SourceA, GetConditionSymbol(Condition), SourceB, Destination);
		}
	}
}
