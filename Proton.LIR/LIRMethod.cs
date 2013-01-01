using System;
using System.Collections.Generic;

namespace Proton.LIR
{
	public sealed class LIRMethod
	{
		internal List<LIRInstruction> mInstructions = new List<LIRInstruction>();
		public IEnumerable<LIRInstruction> Instructions { get { return mInstructions; } }

		internal List<LIRLocal> mLocals = new List<LIRLocal>();
		public List<LIRLocal> Locals { get { return mLocals; } }

		internal List<LIRParameter> mParameters = new List<LIRParameter>();
		public List<LIRParameter> Parameters { get { return mParameters; } }

		public LIRLocal RequestLocal(LIRType tp)
		{
			return new LIRLocal(this, tp);
		}

		public void ReleaseLocal(LIRLocal l)
		{

		}

		public void Dump(IndentedStreamWriter pWriter)
		{
			pWriter.Write("LIRMethod(");
			for (int index = 0; index < mParameters.Count; ++index)
			{
				mParameters[index].Dump(pWriter);
				if (index != mParameters.Count - 1)
					pWriter.Write(", ");
			}
			pWriter.WriteLine(")");
			pWriter.WriteLine("{");
			pWriter.Indent++;
			mInstructions.ForEach(i => i.Dump(pWriter));
			pWriter.Indent--;
			pWriter.WriteLine("}");
		}
	}
}
