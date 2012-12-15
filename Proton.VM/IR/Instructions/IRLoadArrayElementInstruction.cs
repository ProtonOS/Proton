using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRLoadArrayElementInstruction : IRInstruction
    {
		private IRType mType = null;
		public IRType Type { get { return mType; } private set { mType = value; } }

        public IRLoadArrayElementInstruction(IRType pType) : base(IROpcode.LoadArrayElement) 
		{
			Type = pType;
		}

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            IRLinearizedLocation source = new IRLinearizedLocation(IRLinearizedLocationType.ArrayElement);
            source.ArrayElement.IndexLocation = new IRLinearizedLocation(pStack.Pop().LinearizedTarget);
			var arraySource = pStack.Pop();
            source.ArrayElement.ArrayLocation = new IRLinearizedLocation(arraySource.LinearizedTarget);
			if (Type == null)
			{
				Type = arraySource.Type.ArrayType;
			}
            source.ArrayElement.ElementType = Type;
            Sources.Add(source);

            IRStackObject result = new IRStackObject();
            result.Type = Type;
            result.LinearizedTarget = new IRLinearizedLocation(IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, Type);
            Destination = new IRLinearizedLocation(result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRLoadArrayElementInstruction(Type), pNewMethod); }

        public override IRInstruction Transform() { return new IRMoveInstruction(this); }

		public override bool Resolved { get { return Type.Resolved; } }
		public override void Resolve()
		{
			base.Resolve();
			Type.Resolve(ref mType, ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters);
		}

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Type {0}", Type.ToString());
		}
	}
}
