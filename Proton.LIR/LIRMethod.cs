using System;
using System.Collections.Generic;

namespace Proton.LIR
{
	public sealed class LIRMethod
	{
		private const int InitialInstructionCapacity = 256;
		private const int InitialLocalCapacity = 32;
		private const int InitialParameterCapacity = 8;

		internal List<LIRInstruction> mInstructions = new List<LIRInstruction>(InitialInstructionCapacity);
		public IEnumerable<LIRInstruction> Instructions { get { return mInstructions; } }

		internal List<LIRLocal> mLocals = new List<LIRLocal>(InitialLocalCapacity);
		public List<LIRLocal> Locals { get { return mLocals; } }

		internal List<LIRParameter> mParameters = new List<LIRParameter>(InitialParameterCapacity);
		public List<LIRParameter> Parameters { get { return mParameters; } }

		public string Name { get; private set; }
		public LIRType ReturnType { get; private set; }

		private static int sTempID = 0;
		private int mTempID;
		public LIRMethod(string name, LIRType returnType)
		{
			this.mTempID = sTempID++;
			this.Name = name;
			this.ReturnType = returnType;
		}

		public override string ToString()
		{
			return String.Format("~{0}:{1}", mTempID, Name);
		}

		public void MarkLabel(Label l)
		{
			l.Index = mInstructions.Count;
			mInstructions.Add(l);
		}

		internal Dictionary<int, Queue<LIRLocal>> requestedLocalMap = new Dictionary<int, Queue<LIRLocal>>();
		public LIRLocal RequestLocal(LIRType tp)
		{
			LIRLocal ret;
			Queue<LIRLocal> q;
			if (!requestedLocalMap.TryGetValue(tp.GetHashCode(), out q))
			{
				requestedLocalMap.Add(tp.GetHashCode(), new Queue<LIRLocal>());
				ret = new LIRLocal(this, tp);
			}
			else if (q.Count > 0)
				ret = q.Dequeue();
			else
			{
				ret = new LIRLocal(this, tp);
			}
			ret.Dynamic = true;
			return ret;
		}
		public void ReleaseLocal(LIRLocal l)
		{
			requestedLocalMap[l.Type.GetHashCode()].Enqueue(l);
		}

		public void Dump(IndentedStreamWriter pWriter)
		{
			pWriter.Write("LIRMethod ~{0}:{1}(", mTempID, Name);
			for (int index = 0; index < mParameters.Count; ++index)
			{
				mParameters[index].Dump(pWriter);
				if (index != mParameters.Count - 1)
					pWriter.Write(", ");
			}
			pWriter.Write(")");
			if (ReturnType != null)
				pWriter.Write(" : {0}", ReturnType);
			pWriter.WriteLine();
			pWriter.WriteLine("{");
			pWriter.Indent++;

			if (mLocals.Count > 0)
			{
				pWriter.WriteLine("{");
				pWriter.Indent++;
				mLocals.ForEach(l => l.Dump(pWriter));
				pWriter.Indent--;
				pWriter.WriteLine("}");
			}

			mInstructions.ForEach(i => i.Dump(pWriter));
			
			pWriter.Indent--;
			pWriter.WriteLine("}");
		}
	}
}
