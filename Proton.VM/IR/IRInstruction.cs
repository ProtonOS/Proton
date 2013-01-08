using System;
using System.Collections.Generic;
using Proton.LIR;

namespace Proton.VM.IR
{
	public abstract class IRInstruction
	{
		public int ILOffset = 0;
		public int IRIndex = 0;
		public IROpcode Opcode = IROpcode.Nop;
		public IRMethod ParentMethod = null;

		// Instruction Linearization
		public bool Linearized = false;
		public List<IRLinearizedLocation> Sources = new List<IRLinearizedLocation>();
		public IRLinearizedLocation Destination = null;

		// LIR transformation stage
		private Label mLabel = null;
		public Label Label { get { return mLabel ?? (mLabel = new Label()); } }

		protected IRInstruction(IROpcode pOpcode)
		{
			Opcode = pOpcode;
		}

		public int AddLinearizedLocal(Stack<IRStackObject> pStack, IRType pType) { return ParentMethod.AddLinearizedLocal(pStack, pType); }

		public abstract void Linearize(Stack<IRStackObject> pStack);


		/// <summary>
		/// This does a shallow copy of all of the members of the
		/// abstract IRInstruction class to the specified instruction.
		/// </summary>
		/// <param name="i">The instruction to copy to.</param>
		/// <param name="newMethod">The method this instruction will be added to.</param>
		/// <returns><see cref="i"/></returns>
		protected IRInstruction CopyTo(IRInstruction i, IRMethod newMethod)
		{
			i.ILOffset = this.ILOffset;
			i.IRIndex = this.IRIndex;
			i.Opcode = this.Opcode;
			i.ParentMethod = newMethod;
			if (Destination != null)
			{
				i.Destination = this.Destination.Clone(i);
			}
			foreach (IRLinearizedLocation t in this.Sources)
			{
				i.Sources.Add(t.Clone(i));
			}
			return i;
		}

		public abstract IRInstruction Clone(IRMethod pNewMethod);
		public virtual bool Resolved { get { return true; } }

		public void Substitute() { Resolve(); }

		public virtual void Resolve()
		{
			if (Destination != null)
				Destination.Resolve();
			Sources.ForEach(s => s.Resolve());
		}

		public virtual IRInstruction Transform() { return this; }

		public abstract void ConvertToLIR(LIRMethod pLIRMethod);

		public void Dump(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("IRInstruction {0}", Opcode.ToString());
			pWriter.WriteLine("{");
			pWriter.Indent++;
			pWriter.WriteLine("IRIndex {0}", IRIndex);
			DumpDetails(pWriter);

			if (Destination != null)
			{
				pWriter.WriteLine("Destination {0}", Destination.Type);
				pWriter.WriteLine("{");
				pWriter.Indent++;
				Destination.Dump(pWriter);
				pWriter.Indent--;
				pWriter.WriteLine("}");
			}
			if (Sources.Count > 0)
			{
				pWriter.WriteLine("Sources");
				pWriter.WriteLine("{");
				pWriter.Indent++;
				for (int index = 0; index < Sources.Count; ++index)
				{
					pWriter.WriteLine("Source {0}", Sources[index].Type);
					pWriter.WriteLine("{");
					pWriter.Indent++;
					Sources[index].Dump(pWriter);
					pWriter.Indent--;
					pWriter.WriteLine("}");
				}
				pWriter.Indent--;
				pWriter.WriteLine("}");
			}

			pWriter.Indent--;
			pWriter.WriteLine("}");
		}

		protected virtual void DumpDetails(IndentableStreamWriter pWriter) { }
	}
}
