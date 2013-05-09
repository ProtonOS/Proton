using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRBoxInstruction : IRInstruction
	{
		private IRType mType = null;
		public IRType Type { get { return mType; } private set { mType = value; } }

		public IRBoxInstruction(IRType pType) : base(IROpcode.Box) { Type = pType; }

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget));

			IRStackObject result = new IRStackObject();
			result.Type = ParentMethod.Assembly.AppDomain.System_Object;
			result.BoxedType = Type;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
			result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, ParentMethod.Assembly.AppDomain.System_Object);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
			pStack.Push(result);
		}

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRBoxInstruction(Type), pNewMethod); }

		public override bool Resolved { get { return Type.Resolved; } }
		public override void Resolve()
		{
			base.Resolve();
			Type.Resolve(ref mType, ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters);
		}

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
			var sValuePtr = pLIRMethod.RequestLocal(Sources[0].GetTypeOfLocation());
			Sources[0].LoadAddressTo(pLIRMethod, sValuePtr);
			var sTypeDataPtr = pLIRMethod.RequestLocal(AppDomain.System_GC_BoxObject.LIRMethod.Parameters[0].Type);
			new LIRInstructions.Move(pLIRMethod, Type.MetadataLabel, sTypeDataPtr, sTypeDataPtr.Type);
			var sReturnPtr = pLIRMethod.RequestLocal(AppDomain.System_GC_BoxObject.LIRMethod.Parameters[2].Type);
			Destination.LoadAddressTo(pLIRMethod, sReturnPtr);
			List<ISource> boxObjectParams = new List<ISource>(3);
			boxObjectParams.Add(sTypeDataPtr); // pointer to type data
			boxObjectParams.Add(sValuePtr); // pointer to value
			boxObjectParams.Add(sReturnPtr); // pointer to destination
			new LIRInstructions.Call(pLIRMethod, AppDomain.System_GC_BoxObject, boxObjectParams, null);
			pLIRMethod.ReleaseLocal(sReturnPtr);
			pLIRMethod.ReleaseLocal(sTypeDataPtr);
			pLIRMethod.ReleaseLocal(sValuePtr);
		}

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Type {0}", Type.ToString());
		}

		public override string ToString()
		{
			return "Box " + Type + " " + Sources[0] + " -> " + Destination;
		}
	}
}
