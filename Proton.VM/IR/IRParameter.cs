using Proton.Metadata.Tables;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR
{
    public sealed class IRParameter
    {
        public IRAssembly Assembly = null;
		private IRMethod mParentMethod = null;
		public IRMethod ParentMethod { get { return mParentMethod; } set { if (value == null) throw new Exception(); mParentMethod = value; } }

		private IRParameter mParentParameter = null;
		private IRType mType;
		public IRType Type
		{
			get
			{
				if (mType == null && mParentParameter != null)
				{
					mType = mParentParameter.Type;
					mParentParameter = null;
				}
				if (mType == null) throw new Exception();
				return mType;
			}
			set
			{
				if (value == null && mParentParameter == null) throw new Exception();
				mType = value;
			}
		}

        public bool Resolved { get { return Type.Resolved; } }

		public void Substitute() { Resolve(); }

        public void Resolve()
        {
            Type.Resolve(ref mType, ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters);
        }

        public IRParameter(IRAssembly pAssembly)
        {
            Assembly = pAssembly;
        }

        public IRParameter Clone(IRMethod newMethod)
        {
            IRParameter p = new IRParameter(this.Assembly);
			p.ParentMethod = newMethod;
			p.mParentParameter = this.mType == null ? this : null;
			p.Type = this.mType;
            return p;
        }

		public override string ToString()
		{
			return Type.ToString();
		}
    }
}
