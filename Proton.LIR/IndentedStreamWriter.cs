using System;
using System.IO;
using System.Collections.Generic;

namespace Proton.LIR
{
	internal sealed class IndentedStreamWriter
	{
		private int mIdent = 0;
		private string mIdentStr = "";
		public int Indent
		{
			get { return mIdent; }
			set
			{
				if (value < 0)
					throw new ArgumentOutOfRangeException("value");
				mIdent = value;
				mIdentStr = new String('\t', mIdent);
			}
		}
		private readonly StreamWriter mRtr;

		public IndentedStreamWriter(StreamWriter internalRtr)
		{
			mRtr = internalRtr;
		}

		private bool mIdented = false;
		public void Write(string str, params object[] formatArgs)
		{
			if (!mIdented)
			{
				mRtr.Write(mIdentStr);
				mIdented = true;
			}
			string r = str;
			if (r != "{" && r != "}")
				r = string.Format(str, formatArgs);
			mRtr.Write(r);
		}

		public void WriteLine()
		{
			if (!mIdented)
			{
				mRtr.Write(mIdentStr);
				mIdented = true;
			}
			mRtr.WriteLine();
			mIdented = false;
		}

		public void WriteLine(string str, params object[] formatArgs)
		{
			if (!mIdented)
			{
				mRtr.Write(mIdentStr);
				mIdented = true;
			}
			string r = str;
			if (r != "{" && r != "}")
				r = string.Format(str, formatArgs);
			mRtr.WriteLine(r);
			mIdented = false;
		}
	}
}
