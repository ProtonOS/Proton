using System;
using System.IO;
using System.Collections.Generic;

namespace Proton.LIR
{
	public sealed class Label
	{
		private static int sTempID = 0;
		private int mTempID;
		public Label()
		{
			mTempID = sTempID++;
		}

		public override int GetHashCode()
		{
			return mTempID;
		}
	}
	public sealed class LIRCompileUnit
	{
		private const int InitialDataSize = 512;
		private const int InitialMethodsSize = 512;

		private Dictionary<Label, byte[]> mData = new Dictionary<Label, byte[]>(InitialDataSize);
		public Dictionary<Label, byte[]> Data { get { return mData; } }

		private List<LIRMethod> mMethods = new List<LIRMethod>(InitialMethodsSize);
		public List<LIRMethod> Methods { get { return mMethods; } }

		public void Compile(Stream output)
		{




			Console.WriteLine("Dumping LIRCompileUnit...");
			using (var v = new StreamWriter("CompileUnitDump.txt"))
			{
				Dump(new IndentedStreamWriter(v));
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
