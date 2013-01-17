using Proton.Metadata.Tables;
using Proton.Metadata.Signatures;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR
{
	public sealed class IRLocal
	{
		public IRAssembly Assembly = null;
		public IRMethod ParentMethod = null;

		private IRType mType;
		public IRType Type
		{
			get { return mType; }
			set
			{
				if (value == null) throw new Exception();
				mType = value;
			}
		}

		public int Index = 0;
		public int Offset = -1;

		public bool Resolved { get { return Type.Resolved; } }

		public void Substitute() { Resolve(); }

		public void Resolve()
		{
			if (Type == ParentMethod.ParentType.GenericType)
				Type = ParentMethod.ParentType;
			else
				Type.Resolve(ref mType, ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters);
		}

		public sealed class IRLocalSSAData
		{
			public IRLocal Original = null;
			public int Iteration = 0;
			public bool Phi = false;
			public IRInstruction LifeBegins = null;
			public IRInstruction LifeEnds = null;

			public IRLocalSSAData(IRLocal pOriginal) { Original = pOriginal; }
			public IRLocalSSAData Clone()
			{
				IRLocalSSAData clone = new IRLocalSSAData(Original);
				clone.Iteration = Iteration;
				clone.LifeBegins = LifeBegins;
				clone.LifeEnds = LifeEnds;
				return clone;
			}
			public bool IsDead { get { return LifeBegins == null || LifeEnds == null; } }
		}
		public sealed class IRLocalReductionData
		{
			public IRInstruction LifeEnds = null;
			public IRLocal Local = null;
			public IRLocalReductionData(IRInstruction pLifeEnds, IRLocal pLocal) { LifeEnds = pLifeEnds; Local = pLocal; }
		}
		public IRLocalSSAData SSAData = null;

		private static int sTempID = 0;
		private int mTempID = 0;
		
		public IRLocal(IRAssembly pAssembly)
		{
			Assembly = pAssembly;
			mTempID = sTempID++;
		}

		public IRLocal Clone(IRMethod newMethod)
		{
			IRLocal local = new IRLocal(this.Assembly);
			local.ParentMethod = newMethod;
			if (this.Type != null)
				local.mType = this.Type;
			local.Index = newMethod.Locals.Count;
			if (SSAData != null) local.SSAData = SSAData.Clone();
			return local;
		}

		public override string ToString()
		{
			string ssaBuf = "";
			if (SSAData != null)
			{
				if (SSAData.IsDead)
				{
					if (SSAData.Phi) ssaBuf = " (Dead Phi)";
					else ssaBuf = " (Dead)";
				}
				else if (SSAData.Phi) ssaBuf = string.Format(" (Alive {0}-{1} Phi)", SSAData.LifeBegins.IRIndex, SSAData.LifeEnds.IRIndex);
				else ssaBuf = string.Format(" (Alive {0}-{1})", SSAData.LifeBegins.IRIndex, SSAData.LifeEnds.IRIndex);
			}
			return Type.ToString() + ": " + Index.ToString() + ssaBuf;
		}

		public void Dump(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("IRLocal #{0} {1} @ {2}", mTempID, ToString(), Offset);
		}
	}
}
