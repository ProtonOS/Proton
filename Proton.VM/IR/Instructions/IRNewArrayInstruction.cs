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
			var sElementCount = pLIRMethod.RequestLocal(Sources[0].GetTypeOfLocation());
			Sources[0].LoadTo(pLIRMethod, sElementCount);
			var sTypeDataPtr = pLIRMethod.RequestLocal(AppDomain.System_GC_AllocateArrayOfType.LIRMethod.Parameters[0].Type);
			new LIRInstructions.Move(pLIRMethod, Destination.GetTypeOfLocation().MetadataLabel, sTypeDataPtr, sTypeDataPtr.Type);
			var sReturnPtr = pLIRMethod.RequestLocal(AppDomain.System_GC_AllocateArrayOfType.LIRMethod.Parameters[2].Type);
			Destination.LoadAddressTo(pLIRMethod, sReturnPtr);
			List<ISource> allocateArrayOfTypeParams = new List<ISource>(3);
			allocateArrayOfTypeParams.Add(sTypeDataPtr); // pointer to type data of array
			allocateArrayOfTypeParams.Add(sElementCount); // number of elements
			allocateArrayOfTypeParams.Add(sReturnPtr); // pointer to destination
			new LIRInstructions.Call(pLIRMethod, AppDomain.System_GC_AllocateArrayOfType, allocateArrayOfTypeParams, null);
			pLIRMethod.ReleaseLocal(sReturnPtr);
			pLIRMethod.ReleaseLocal(sTypeDataPtr);
			pLIRMethod.ReleaseLocal(sElementCount);
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
