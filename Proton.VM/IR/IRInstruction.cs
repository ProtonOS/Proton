using System;
using System.Collections.Generic;

namespace Proton.VM.IR
{
    public abstract class IRInstruction
    {
        public int ILOffset = 0;
        public uint IRIndex = 0;
        public IROpcode Opcode = IROpcode.Nop;
        public IRMethod ParentMethod = null;

        // Instruction Linearization
        public bool Linearized = false;
        public List<IRLinearizedLocation> Sources = new List<IRLinearizedLocation>();
        public IRLinearizedLocation Destination = null;

        protected IRInstruction(IROpcode pOpcode)
        {
            Opcode = pOpcode;
        }

        public uint AddLinearizedLocal(Stack<IRStackObject> pStack, IRType pType) { return ParentMethod.AddLinearizedLocal(pStack, pType); }

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
				i.Destination = this.Destination.Clone();
			}
            foreach (IRLinearizedLocation t in this.Sources)
            {
                i.Sources.Add(t.Clone());
            }
            return i;
        }

        public abstract IRInstruction Clone(IRMethod pNewMethod);
        public virtual bool Resolved { get { return true; } }

		public void Substitute() { Resolve(); }

        public virtual void Resolve()
        {
            if (Destination != null)
				Destination.Resolve(ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters);
			Sources.ForEach(s => s.Resolve(ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters));
        }

        public virtual IRInstruction Transform() { return this; }
    }
}
