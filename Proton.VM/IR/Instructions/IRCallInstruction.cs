using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRCallInstruction : IRInstruction
	{
		private IRMethod mTarget = null;
		public IRMethod Target { get { return mTarget; } set { mTarget = value; } }
		public bool Virtual { get; set; }

		public IRCallInstruction(IRMethod pTarget, bool pVirtual) : base(IROpcode.Call)
		{
			Target = pTarget;
			Virtual = pVirtual;
		}

		private static IRType ResolveSimpleReturn(IRType tp, IRMethod target)
		{
			if (tp.IsTemporaryVar)
				return target.ParentType.GenericParameters[tp.TemporaryVarOrMVarIndex];
			else if (tp.IsTemporaryMVar)
				return target.GenericParameters[tp.TemporaryVarOrMVarIndex];
			else if (tp.IsArrayType)
				return target.Assembly.AppDomain.GetArrayType(ResolveSimpleReturn(tp.ArrayElementType, target));
			else
				return tp;
		}

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			for (int count = 0; count < Target.Parameters.Count; ++count) Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget));

			if (Target.ReturnType != null)
			{
				IRStackObject returned = new IRStackObject();
				IRType retType = ResolveSimpleReturn(Target.ReturnType, Target);

				returned.Type = retType;
				returned.LinearizedTarget = new IRLinearizedLocation(this, IRLinearizedLocationType.Local);
				returned.LinearizedTarget.Local.LocalIndex = AddLinearizedLocal(pStack, retType);
				Destination = new IRLinearizedLocation(this, returned.LinearizedTarget);
				pStack.Push(returned);
			}
		}

		public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRCallInstruction(Target, Virtual), pNewMethod); }

		public override bool Resolved { get { return Target == ParentMethod ? true : Target.Resolved; } }
		public override void Resolve()
		{
			base.Resolve();
			Target.Resolve(ref mTarget, ParentMethod.ParentType.GenericParameters, ParentMethod.GenericParameters);
		}

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
			if (Virtual)
			{
				if (Target.VirtualMethodIndex < 0) throw new Exception();
				List<LIRLocal> paramSources = new List<LIRLocal>(Target.Parameters.Count);
				foreach (var v in Sources)
				{
					var s = pLIRMethod.RequestLocal(v.GetTypeOfLocation());
					v.LoadTo(pLIRMethod, s);
					paramSources.Add(s);
				}
				ISource methodPointerSource = null;
				if (!Target.ParentType.IsInterface)
				{
					var s = pLIRMethod.RequestLocal(Sources[0].GetTypeOfLocation());
					Sources[0].LoadTo(pLIRMethod, s); // Gets Object Pointer
					new LIRInstructions.Math(pLIRMethod, s, (LIRImm)VMConfig.PointerSizeForTarget, s, LIRInstructions.MathOperation.Subtract, s.Type); // Subtract sizeof pointer to get address of TypeData pointer
					new LIRInstructions.Move(pLIRMethod, new Indirect(s), s, s.Type); // Get TypeData Pointer
					new LIRInstructions.Move(pLIRMethod, new Indirect(s), s, s.Type); // Get VirtualMethodTree Pointer Array
					new LIRInstructions.Math(pLIRMethod, s, (LIRImm)(VMConfig.PointerSizeForTarget * Target.VirtualMethodIndex), s, LIRInstructions.MathOperation.Add, s.Type); // Get VirtualMethod Pointer Address
					new LIRInstructions.Move(pLIRMethod, new Indirect(s), s, s.Type); // Get VirtualMethod Pointer
					methodPointerSource = s;
				}
				else
				{
#warning Interface Calls Not Yet Implemented.
					return;
				}
				if (Target.ReturnType != null)
				{
					var dest = pLIRMethod.RequestLocal(Destination.GetTypeOfLocation());
					new LIRInstructions.CallIndirect(pLIRMethod, methodPointerSource, paramSources, dest);
					Destination.StoreTo(pLIRMethod, dest);
					pLIRMethod.ReleaseLocal(dest);
				}
				else
				{
					new LIRInstructions.CallIndirect(pLIRMethod, methodPointerSource, paramSources);
				}
				paramSources.ForEach(s => pLIRMethod.ReleaseLocal(s));
			}
			else
			{
				List<LIRLocal> paramSources = new List<LIRLocal>(Target.Parameters.Count);
				foreach (var v in Sources)
				{
					var s = pLIRMethod.RequestLocal(v.GetTypeOfLocation());
					v.LoadTo(pLIRMethod, s);
					paramSources.Add(s);
				}
				if (Target.ReturnType != null)
				{
					var dest = pLIRMethod.RequestLocal(Destination.GetTypeOfLocation());
					new LIRInstructions.Call(pLIRMethod, Target.LIRMethod, paramSources, dest);
					Destination.StoreTo(pLIRMethod, dest);
					pLIRMethod.ReleaseLocal(dest);
				}
				else
				{
					new LIRInstructions.Call(pLIRMethod, Target.LIRMethod, paramSources);
				}
				paramSources.ForEach(s => pLIRMethod.ReleaseLocal(s));
			}
		}

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("Target {0}", Target.ToString());
			pWriter.WriteLine("Virtual {0}", Virtual);
		}

		public override string ToString()
		{
			if (Virtual)
			{
				if (Target.ReturnType != null)
					return "Call Virtual " + Target.ToString(false) + "(" + String.Join(", ", Sources) + ") -> " + Destination;
				else
					return "Call Virtual " + Target.ToString(false) + "(" + String.Join(", ", Sources) + ")";
			}
			else
			{
				if (Target.ReturnType != null)
					return "Call " + Target.ToString(false) + "(" + String.Join(", ", Sources) + ") -> " + Destination;
				else
					return "Call " + Target.ToString(false) + "(" + String.Join(", ", Sources) + ")";
			}
			
		}
	}
}
