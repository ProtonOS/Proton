using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRNewObjectInstruction : IRInstruction
	{
		private IRMethod mConstructor = null;
		public IRMethod Constructor { get { return mConstructor; } private set { mConstructor = value; } }

		public IRNewObjectInstruction(IRMethod pConstructor) : base(IROpcode.NewObject) { Constructor = pConstructor; }

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			// Skip first parameter source, it is the 'this' reference that is put on the stack by newobj before constructor call
			for (int count = 1; count < Constructor.Parameters.Count; ++count) Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget));

			IRStackObject result = new IRStackObject();
			result.Type = Constructor.ParentType;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
			result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, Constructor.ParentType);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
			pStack.Push(result);
		}

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRNewObjectInstruction(Constructor), pNewMethod); }

		public override bool Resolved { get { return Constructor.Resolved; } }
		public override void Resolve()
		{
			base.Resolve();
			Constructor.Resolve(ref mConstructor, ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters);
		}

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
			if (Constructor.ParentType == AppDomain.System_String)
			{
#warning Finish special cases for string constructors
			}
			else
			{
				var sTypeDataPtr = pLIRMethod.RequestLocal(AppDomain.System_GC_AllocateObject.LIRMethod.Parameters[0].Type);
				new LIRInstructions.Move(pLIRMethod, Constructor.ParentType.TypeDataLabel, sTypeDataPtr, sTypeDataPtr.Type);
				var sReturnPtr = pLIRMethod.RequestLocal(AppDomain.System_GC_AllocateObject.LIRMethod.Parameters[1].Type);
				Destination.LoadAddressTo(pLIRMethod, sReturnPtr);
				List<ISource> allocateObjectParams = new List<ISource>(2);
				allocateObjectParams.Add(sTypeDataPtr); // pointer to type data
				allocateObjectParams.Add(sReturnPtr); // pointer to destination
				new LIRInstructions.Call(pLIRMethod, AppDomain.System_GC_AllocateObject, allocateObjectParams, null);
				pLIRMethod.ReleaseLocal(sReturnPtr);
				pLIRMethod.ReleaseLocal(sTypeDataPtr);

				List<LIRLocal> constructorParams = new List<LIRLocal>(Sources.Count + 1);
				var sObj = pLIRMethod.RequestLocal(Destination.GetTypeOfLocation());
				Destination.LoadTo(pLIRMethod, sObj);
				constructorParams.Add(sObj);
				foreach (IRLinearizedLocation sourceLocation in Sources)
				{
					var s = pLIRMethod.RequestLocal(sourceLocation.GetTypeOfLocation());
					constructorParams.Add(s);
					sourceLocation.LoadTo(pLIRMethod, s);
				}
				new LIRInstructions.Call(pLIRMethod, Constructor, constructorParams, null);
				constructorParams.ForEach(s => pLIRMethod.ReleaseLocal(s));
			}
		}

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Constructor {0}", Constructor.ToString());
		}

		public override string ToString()
		{
			return "NewObject " + Constructor.ToString(false) + "(" + String.Join(", ", Sources) + ") -> " + Destination;
		}
	}
}
