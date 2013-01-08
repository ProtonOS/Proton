using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRNewArrayInstruction : IRInstruction
	{
		private IRType mElementType = null;
		public IRType ElementType { get { return mElementType; } private set { mElementType = value; } }

		public IRNewArrayInstruction(IRType pElementType) : base(IROpcode.NewArray) { ElementType = pElementType; }

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget));

			IRType arrayType = ParentMethod.Assembly.AppDomain.GetArrayType(ElementType);
			IRStackObject result = new IRStackObject();
			result.Type = arrayType;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
			result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, arrayType);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
			pStack.Push(result);
		}

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRNewArrayInstruction(ElementType), pNewMethod); }

		public override bool Resolved { get { return ElementType.Resolved; } }
		public override void Resolve()
		{
			base.Resolve();
			ElementType.Resolve(ref mElementType, ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters);
		}

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
		}

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Type {0}", ElementType.ToString());
		}

		public override string ToString()
		{
			return "NewArray " + ElementType + " " + Sources[0] + " -> " + Destination;
		}
	}
}
