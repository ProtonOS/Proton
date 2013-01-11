using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using Proton.Metadata.Tables;
using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRUnboxInstruction : IRInstruction
	{
		public IRType Type = null;
		public bool GetValue = false;

		public IRUnboxInstruction(IRType pType, bool pValue) : base(IROpcode.Unbox)
		{
			Type = pType;
			GetValue = pValue;
		}

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget));

			IRType resultType = Type;
			if (!GetValue) resultType = ParentMethod.Assembly.AppDomain.GetUnmanagedPointerType(Type);
			IRStackObject result = new IRStackObject();
			result.Type = resultType;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
			result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, resultType);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
			pStack.Push(result);
		}

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRUnboxInstruction(Type, GetValue), pNewMethod); }

		public override bool Resolved { get { return Type.Resolved; } }
		public override void Resolve()
		{
			base.Resolve();
			Type.Resolve(ref Type, ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters);
		}

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
			var s = pLIRMethod.RequestLocal(Sources[0].GetTypeOfLocation());
			Sources[0].LoadTo(pLIRMethod, s);
			if (GetValue)
			{
				new LIRInstructions.Move(pLIRMethod, new Indirect(s), s, Destination.GetTypeOfLocation());
			}
			Destination.StoreTo(pLIRMethod, s);
			pLIRMethod.ReleaseLocal(s);
		}

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Type {0}", Type.ToString());
			pWriter.WriteLine("GetValue {0}", GetValue);
		}

		public override string ToString()
		{
			return "UnBox " + (GetValue ? "ValueOf " : "AddressOf ") + Type + " " + Sources[0] + " -> " + Destination;
		}
	}
}
