using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using Proton.LIR;

namespace Proton.VM
{
	public sealed class IndentableStreamWriter
	{
		private StreamWriter mWriter = null;
		private int mIndent = 0;

		public IndentableStreamWriter(string pPath) { mWriter = new StreamWriter(pPath); }

		public int Indent { get { return mIndent; } set { mIndent = value; } }

		public void Close() { mWriter.Close(); }

		public void WriteLine(string pFormat, params object[] pArgs)
		{
			mWriter.WriteLine(new string('\t', mIndent) + ((pFormat == "}" || pFormat == "{") ? pFormat : string.Format(pFormat, pArgs)));
		}

		public static implicit operator IndentedStreamWriter(IndentableStreamWriter r)
		{
			return new IndentedStreamWriter(r.mWriter) { Indent = r.mIndent };
		}
	}
}
