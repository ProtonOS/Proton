using Proton.Metadata.Tables;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR
{
    public sealed class IRParameter
    {
        public IRAssembly Assembly = null;
        public IRMethod ParentMethod = null;

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
				return mType;
			}
			set
			{
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
			p.Type = this.Type;
			p.mParentParameter = this.Type == null ? this : null;
            return p;
        }

		public override string ToString()
		{
			return Type.ToString();
		}
    }
}
