using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRCastInstruction : IRInstruction
	{
		private IRType mType = null;
		public IRType Type { get { return mType; } private set { mType = value; } }
		public bool ThrowExceptionOnFailure { get; private set; }

		public IRCastInstruction(IRType pType, bool pThrowExceptionOnFailure) : base(IROpcode.Cast)
		{
			Type = pType;
			ThrowExceptionOnFailure = pThrowExceptionOnFailure;
		}

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget));

			IRStackObject result = new IRStackObject();
			result.Type = Type;
			result.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
			result.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, Type);
			Destination = new IRLinearizedLocation(this, result.LinearizedTarget);
			pStack.Push(result);
		}

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRCastInstruction(Type, ThrowExceptionOnFailure), pNewMethod); }

		public override bool Resolved { get { return Type.Resolved; } }
		public override void Resolve()
		{
			base.Resolve();
			Type.Resolve(ref mType, ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters);
		}

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
			var src = pLIRMethod.RequestLocal(Sources[0].GetTypeOfLocation());
			Sources[0].LoadTo(pLIRMethod, src);
			var dest = pLIRMethod.RequestLocal(Destination.GetTypeOfLocation());
			var canCastResult = pLIRMethod.RequestLocal(ParentMethod.Assembly.AppDomain.System_Boolean);
			var getTypeByGlobalTypeIDResult = pLIRMethod.RequestLocal(ParentMethod.Assembly.AppDomain.System_Type);

			// Call to Proton.Runtime.RuntimeHelpers.GetTypeByGlobalTypeID method, which returns the runtime System.Type
			new LIRInstructions.Call(pLIRMethod, ParentMethod.Assembly.AppDomain.Proton_Runtime_RuntimeHelpers_GetTypeByGlobalTypeID.LIRMethod, new List<ISource>(1) { (LIRImm)Type.GlobalTypeID }, getTypeByGlobalTypeIDResult);

			// Call to Proton.Runtime.RuntimeHelpers.CanCast method, which determines if an object can be cast to a type
			// TODO: can src be a value type? if so then we may need to box it first to pass it into this call as an object
			// TODO: this method should throw an exception if the Type is null, so we can skip testing castType before the call
			new LIRInstructions.Call(pLIRMethod, ParentMethod.Assembly.AppDomain.Proton_Runtime_RuntimeHelpers_CanCast.LIRMethod, new List<ISource>(2) { src, getTypeByGlobalTypeIDResult }, canCastResult);

			// Don't need getTypeByGlobalTypeIDResult anymore, we have canCastResult
			pLIRMethod.ReleaseLocal(getTypeByGlobalTypeIDResult);

			// Branch if we can cast, fall through if we cannot
			LIRInstructions.BranchTrue canCastBranchTrue = new LIRInstructions.BranchTrue(pLIRMethod, canCastResult);

			// Getting here means CanCast returned false, so the emitted code depends on ThrowExceptionOnFailure
			if (!ThrowExceptionOnFailure)
			{
				// If we don't throw an exception, then we are expected to set the destination to it's default value (null, 0, etc)
				new LIRInstructions.Move(pLIRMethod, dest.Type.Empty, dest, dest.Type);
			}
			// Now we jump past the previous BranchTrue logic below
			LIRInstructions.Branch cannotCastBranchOut = new LIRInstructions.Branch(pLIRMethod);
			// Getting here means CanCast returned true, so we need to set the BranchTrue target label
			canCastBranchTrue.SetTarget(new Label());
			// And now copy src to dest, based on the destination casted type
			// TODO: Is it safe to use src here, after it was passed as a source to the Call above?
			new LIRInstructions.Move(pLIRMethod, src, dest, dest.Type);

			cannotCastBranchOut.SetTarget(new Label());

			// We're done with src now, clean that up
			pLIRMethod.ReleaseLocal(src);

			if (!ThrowExceptionOnFailure)
			{
				// If we don't throw an exception, then we end up moving null to destination
				// so we can just do a normal cleanup and store here
				pLIRMethod.ReleaseLocal(canCastResult);
				Destination.StoreTo(pLIRMethod, dest);
				pLIRMethod.ReleaseLocal(dest);
			}
			else
			{
				// Branch if we can cast, fall through if we cannot
				canCastBranchTrue = new LIRInstructions.BranchTrue(pLIRMethod, canCastResult);
				// Getting here means we were not able to cast, so we need to cleanup before we throw an
				// exception, but we do not store anything to the destination
				pLIRMethod.ReleaseLocal(canCastResult);
				pLIRMethod.ReleaseLocal(dest);

				// TODO: Throw exception here, Branch is here to loop infinite, but this is not valid
				// it is here only as a placeholder, we need to prepare the object being thrown in a
				// way that the reference is held until an exception handler has the reference
				Label loopLabel = new Label();
				new LIRInstructions.Comment(pLIRMethod, "TODO: Replace this infinite loop with Throw logic");
				new LIRInstructions.Branch(pLIRMethod, loopLabel);

				// Getting here means we were able to cast, just do a normal cleanup
				canCastBranchTrue.SetTarget(new Label());
				pLIRMethod.ReleaseLocal(canCastResult);
				Destination.StoreTo(pLIRMethod, dest);
				pLIRMethod.ReleaseLocal(dest);
			}
			// Fall through to next IR instruction if we had a successful cast, or failed without
			// throwing an exception
		}

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Type {0}", Type.ToString());
			pWriter.WriteLine("ThrowExceptionOnFailure {0}", ThrowExceptionOnFailure);
		}

		public override string ToString()
		{
			return "Cast " + (ThrowExceptionOnFailure ? "ThrowOnFail " : "") + Type + " " + Sources[0] + " -> " + Destination;
		}
	}
}
