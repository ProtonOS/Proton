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

		private IRLocal mParentLocal = null;
		private IRType mType;
		public IRType Type
		{
			get
			{
				if (mType == null && mParentLocal != null)
				{
					mType = mParentLocal.Type;
					if (mType == null) throw new Exception();
					mParentLocal = null;
				}
				return mType;
			}
			set
			{
				if (value == null) throw new Exception();
				mType = value;
			}
		}

        public int Index = 0;

        public bool Resolved { get { return Type.Resolved; } }

		public void Substitute() { Resolve(); }

        public void Resolve()
        {
			Type.Resolve(ref mType, ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters);
        }

		public sealed class IRLocalSSAData
		{
			public IRLocal Original = null;
			public int Iteration = 0;
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
		}
		public IRLocalSSAData SSAData = null;

        public IRLocal(IRAssembly pAssembly)
        {
            Assembly = pAssembly;
        }

        public IRLocal Clone(IRMethod newMethod)
        {
            IRLocal local = new IRLocal(this.Assembly);
			local.ParentMethod = newMethod;
			local.mParentLocal = this.Type == null ? this : null;
			if (this.Type != null)
				local.mType = this.Type;
            local.Index = newMethod.Locals.Count;
			if (SSAData != null) local.SSAData = SSAData.Clone();
            return local;
        }

		public override string ToString()
		{
			return Type.ToString() + ": " + Index.ToString();
		}
    }
}
