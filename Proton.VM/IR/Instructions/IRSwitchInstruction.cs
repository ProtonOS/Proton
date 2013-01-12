using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
	public sealed class IRSwitchInstruction : IRInstruction
	{
		public int[] TargetILOffsets = null;
		public IRInstruction[] TargetIRInstructions = null;

		public IRSwitchInstruction(int[] pTargetILOffsets) : base(IROpcode.Switch) { TargetILOffsets = pTargetILOffsets; }

		public override void Linearize(Stack<IRStackObject> pStack)
		{
			Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget));
		}

		public override IRInstruction Clone(IRMethod pNewMethod)
		{
			IRInstruction[] newTargetIRInstructions = (IRInstruction[])TargetIRInstructions.Clone();
			return CopyTo(new IRSwitchInstruction(TargetILOffsets) { TargetIRInstructions = newTargetIRInstructions }, pNewMethod);
		}

		public sealed class SwitchEmittableDataItem : EmittableData
		{
			private Label[] SwitchCases;
			private Label mLabel = new Label("SwitchCaseData");
			public override Label Label { get { return mLabel; } }

			public SwitchEmittableDataItem(params Label[] labels)
			{
				this.SwitchCases = labels;
			}

			public override byte[] GetData(EmissionContext c)
			{
				byte[] ret = new byte[SwitchCases.Length * c.SizeOfLabel];
				for (int i = 0, i2 = 0; i2 < ret.Length; i2 += c.SizeOfLabel, i++)
				{
					Array.Copy(SwitchCases[i].GetData(c), 0, ret, i2, c.SizeOfLabel);
				}
				return ret;
			}

			public override string ToString()
			{
				return String.Format("SwitchCaseData({0})", String.Join(", ", (IEnumerable<Label>)SwitchCases));
			}
		}

		public override void ConvertToLIR(LIRMethod pLIRMethod)
		{
			Label[] lbls = new Label[TargetIRInstructions.Length];
			for (int i = 0; i < lbls.Length; i++)
			{
				lbls[i] = TargetIRInstructions[i].Label;
			}
			var swDatItem = new SwitchEmittableDataItem(lbls);
			pLIRMethod.CompileUnit.AddData(swDatItem);

			// Need to load and compare original value first...
			var vs = pLIRMethod.RequestLocal(Sources[0].GetTypeOfLocation());
			Sources[0].LoadTo(pLIRMethod, vs);
			var vt = pLIRMethod.RequestLocal(AppDomain.System_Boolean);
			new LIRInstructions.Compare(pLIRMethod, vs, (LIRImm)0, vt, vs.Type, LIRInstructions.CompareCondition.LessThan);
			new LIRInstructions.BranchTrue(pLIRMethod, vt, ParentMethod.Instructions[this.IRIndex + 1].Label);
			new LIRInstructions.Compare(pLIRMethod, vs, (LIRImm)lbls.Length, vt, vs.Type, LIRInstructions.CompareCondition.GreaterThan);
			new LIRInstructions.BranchTrue(pLIRMethod, vt, ParentMethod.Instructions[this.IRIndex + 1].Label);
			pLIRMethod.ReleaseLocal(vt);
			var sBase = pLIRMethod.RequestLocal(AppDomain.System_UIntPtr);
			new LIRInstructions.Move(pLIRMethod, swDatItem.Label, sBase, AppDomain.System_UIntPtr);
			new LIRInstructions.Math(pLIRMethod, vs, (LIRImm)VMConfig.PointerSizeForTarget, vs, LIRInstructions.MathOperation.Multiply, vs.Type);
			new LIRInstructions.Math(pLIRMethod, vs, sBase, sBase, LIRInstructions.MathOperation.Add, sBase.Type);
			pLIRMethod.ReleaseLocal(vs);
			new LIRInstructions.Move(pLIRMethod, new Indirect(sBase), sBase, sBase.Type);
			new LIRInstructions.BranchIndirect(pLIRMethod, sBase);
			pLIRMethod.ReleaseLocal(sBase);
		}

		public override string ToString()
		{
			string[] strs = new string[TargetIRInstructions.Length];
			for (int index = 0; index < TargetIRInstructions.Length; ++index) strs[index] = TargetIRInstructions[index].IRIndex.ToString();
			return "Switch " + Sources[0] + "(" + String.Join(", ", strs) + ")";
		}
	}
}
