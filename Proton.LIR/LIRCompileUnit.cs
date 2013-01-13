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
		public string Name { get; private set; }

		private static int sTempID = 0;
		private int mTempID;
		public Label() : base(null, LIROpCode.Label)
		{
			mTempID = sTempID++;
		}

		public Label(string name) : this()
		{
			this.Name = name + "_" + mTempID;
		}

		public override int GetHashCode()
		{
			return mTempID;
		}

		public override string ToString()
		{
			return Name ?? mTempID.ToString();
		}

		public byte[] GetData(EmissionContext context)
		{
			return new byte[4];
		}

		internal override void Dump(IndentedStreamWriter wtr)
		{
			wtr.Indent--;
			//wtr.WriteLine("{0}: ({1} References)", this, References);
			wtr.WriteLine("{0}:", this);
			wtr.Indent++;
		}
	}

	public sealed class EmissionContext
	{
#warning Do me correctly at some point.... (same with Label.GetData)
		public int SizeOfLabel { get { return 4; } }
	}

	public abstract class EmittableData
	{
		private int mPriority;
		public int Priority { get { return mPriority; } }

		protected EmittableData(int priority = 50)
		{
			mPriority = priority;
		}

		public abstract Label Label { get; }

		public abstract byte[] GetData(EmissionContext context);
	}
	public sealed class LIRCompileUnit
	{
		private const int InitialDataSize = 512;
		private const int InitialMethodsSize = 512;

		private Dictionary<Label, EmittableData> mData = new Dictionary<Label, EmittableData>(InitialDataSize);
		public IEnumerable<EmittableData> Data { get { return mData.Values; } }

		private List<LIRMethod> mMethods = new List<LIRMethod>(InitialMethodsSize);
		public IEnumerable<LIRMethod> Methods { get { return mMethods; } }

		public void AddMethod(LIRMethod m)
		{
			mMethods.Add(m);
			m.CompileUnit = this;
		}

		public void AddData(EmittableData data)
		{
			mData.Add(data.Label, data);
		}

		public void Compile(Stream output)
		{
			// Need to align all output to what makes best sense for the architecture (multi-boot must be 4-byte aligned)


			Optimize();

			Console.WriteLine("Dumping LIRCompileUnit...");
			using (var v = new StreamWriter("CompileUnitDump.txt"))
			{
				Dump(new IndentedStreamWriter(v));
			}

			// A bit of cleanup before we do the actual emission.
			mMethods.ForEach(m => m.requestedLocalMap = null); 
			mMethods.ForEach(m => FinalMethodCleanup(m));
			GC.Collect();
			GC.WaitForPendingFinalizers();
			GC.Collect();

			// Now we transform it to MIR and emit it.
		}

		private static void FinalMethodCleanup(LIRMethod m)
		{
			for (int i = 0; i < m.mInstructions.Count; i++)
			{
				if (m.mInstructions[i].OpCode == LIROpCode.Comment)
					m.mInstructions[i] = new Instructions.Dead(i);
			}
			KnownOptimizationPasses[KnownOptimizationPasses.Length - 1].Run(m);
		}

		private static readonly LIROptimizationPass[] KnownOptimizationPasses = new LIROptimizationPass[]
		{
			new LIRDeadLabelRemovalOptimizationPass(),
			new LIRUselessMathRemovalOptimizationPass(),
			//new LIRLimitedMoveCompactingOptimizationPass(),
			new LIRDeadInstructionDestructionOptimizationPass(),
		};

		private void Optimize()
		{
			foreach (var kop in KnownOptimizationPasses)
			{
				foreach (var m in Methods)
				{
					kop.Run(m);
				}
			}
		}

		private void Dump(IndentedStreamWriter wtr)
		{
			wtr.WriteLine("LIRCompileUnit {0}", mMethods.Count + mData.Count);
			wtr.WriteLine("{");
			wtr.Indent++;
			wtr.WriteLine("Data {0}", mData.Count);
			wtr.WriteLine("{");
			wtr.Indent++;
			foreach (var d in mData)
			{
				wtr.WriteLine("{0} -> {1}", d.Key, d.Value);
			}
			wtr.Indent--;
			wtr.WriteLine("}");
			wtr.WriteLine("Methods {0}", mMethods.Count);
			wtr.WriteLine("{");
			wtr.Indent++;
			foreach (var m in Methods)
			{
				m.Dump(wtr);
			}
			wtr.Indent--;
			wtr.WriteLine("}");
			wtr.Indent--;
			wtr.WriteLine("}");
		}
	}
}
