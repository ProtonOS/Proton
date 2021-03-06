﻿using System;
using System.Collections.Generic;

namespace Proton.LIR.Instructions
{
	public sealed class Comment : LIRInstruction
	{
		internal override bool MayHaveSideEffects { get { return false; } }
		public string Text { get; private set; }

		public Comment(LIRMethod parent, string text) : base(parent, LIROpCode.Comment)
		{
			this.Text = text;
		}

		internal override void Dump(IndentedStreamWriter wtr)
		{
			wtr.WriteLine("// {0}", Text);
		}
	}
}
