using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRNewArrayInstruction : IRInstruction
    {
		private IRType mType = null;
		public IRType Type { get { return mType; } private set { mType = value; } }

        public IRNewArrayInstruction(IRType pType) : base(IROpcode.NewArray) { Type = pType; }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            Sources.Add(new IRLinearizedLocation(pStack.Pop().LinearizedTarget));

            IRType arrayType = ParentMethod.Assembly.AppDomain.GetArrayType(Type);
            IRStackObject result = new IRStackObject();
            result.Type = arrayType;
            result.LinearizedTarget = new IRLinearizedLocation(IRLinearizedLocationType.Local);
            result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, arrayType);
            Destination = new IRLinearizedLocation(result.LinearizedTarget);
            pStack.Push(result);
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRNewArrayInstruction(Type), pNewMethod); }

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
