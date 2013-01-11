using System;
using System.IO;
using System.Collections.Generic;
using Proton.LIR.Optimizations;

namespace Proton.LIR
{
	public sealed class Label : LIRInstruction, ISource
	{
		internal override bool MayHaveSideEffects { get { return true; } }
		bool ISource.MayHaveSideEffects { get { return false; } }
		public SourceType SourceType { get { return SourceType.Label; } }
		public int References { get; internal set; }

		private static int sTempID = 0;
		private int mTempID;
		public Label() : base(null, LIROpCode.Label)
		{
			mTempID = sTempID++;
		}

		public override int GetHashCode()
		{
			return mTempID;
		}

		public override string ToString()
		{
			return mTempID.ToString();
		}

		internal override void Dump(IndentedStreamWriter wtr)
		{
			wtr.Indent--;
			//wtr.WriteLine("{0}: ({1} References)", this, References);
			wtr.WriteLine("{0}:", this);
			wtr.Indent++;
		}
	}

	public abstract class EmittableData
	{
		private int mPriority;
		public int Priority { get { return mPriority; } }

		protected EmittableData(int priority = 5)
		{
			mPriority = priority;
		}

		public abstract byte[] GetData();
	}
	public sealed class LIRCompileUnit
	{
		private const int InitialDataSize = 512;
		private const int InitialMethodsSize = 512;

		private Dictionary<Label, EmittableData> mData = new Dictionary<Label, EmittableData>(InitialDataSize);
		public Dictionary<Label, EmittableData> Data { get { return mData; } }

		private List<LIRMethod> mMethods = new List<LIRMethod>(InitialMethodsSize);
		public List<LIRMethod> Methods { get { return mMethods; } }

		public void Compile(Stream output)
		{
			// A bit of cleanup before we do the actual emission.
			mMethods.ForEach(m => m.requestedLocalMap = null);
			// Need to align all output to what makes best sense for the architecture (multi-boot must be 4-byte aligned)


			Optimize();

			Console.WriteLine("Dumping LIRCompileUnit...");
			using (var v = new StreamWriter("CompileUnitDump.txt"))
			{
				Dump(new IndentedStreamWriter(v));
			}



			// Now we trasform it to MIR and emit it.
		}

		private static readonly LIROptimizationPass[] KnownOptimizationPasses = new LIROptimizationPass[]
		{
			new LIRDeadLabelRemovalOptimizationPass(),
			new LIRLimitedMoveCompactingOptimizationPass(),
			new LIRDeadInstructionDestructionOptimizationPass(),
		};

		private void Optimize()
		{
			foreach (var kop in KnownOptimizationPasses)
			{
				mMethods.ForEach(m => kop.Run(m));
			}
		}

		private void Dump(IndentedStreamWriter wtr)
		{
			wtr.WriteLine("LIRCompileUnit {0}", Methods.Count);
			wtr.WriteLine("{");
			wtr.Indent++;
			Methods.ForEach(m => m.Dump(wtr));
			wtr.Indent--;
			wtr.WriteLine("}");
		}
	}
}
