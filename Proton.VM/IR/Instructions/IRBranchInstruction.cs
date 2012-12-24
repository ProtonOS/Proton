using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Instructions
{
    public sealed class IRBranchInstruction : IRInstruction
    {
        private IRBranchCondition mBranchCondition = IRBranchCondition.Always;
        public IRBranchCondition BranchCondition
        {
            get { return mBranchCondition; }
            set { mBranchCondition = value; }
        }
        
        public int TargetILOffset { get; set; }
        public IRInstruction TargetIRInstruction { get; set; }
        public int StackDepth { get; set; }

        public IRBranchInstruction(IRBranchCondition pBranchCondition, int pTargetILOffset) : base(IROpcode.Branch)
        {
            BranchCondition = pBranchCondition;
            TargetILOffset = pTargetILOffset;
        }

        public override void Linearize(Stack<IRStackObject> pStack)
        {
            switch (BranchCondition)
            {
                case IRBranchCondition.Always:
                    //if (pStack.Count > 0)
                    //{
                    //    int sd = Method.StackDepths.Pop();
                    //    while (pStack.Count > sd)
                    //        pStack.Pop();
                    //}
                    break;
                case IRBranchCondition.False:
				case IRBranchCondition.True: Sources.Add(new IRLinearizedLocation(this, pStack.Pop().LinearizedTarget)); break;
                case IRBranchCondition.Equal:
                case IRBranchCondition.GreaterOrEqual:
                case IRBranchCondition.GreaterOrEqualUnsigned:
                case IRBranchCondition.Greater:
                case IRBranchCondition.GreaterUnsigned:
                case IRBranchCondition.LessOrEqual:
                case IRBranchCondition.LessOrEqualUnsigned:
                case IRBranchCondition.Less:
                case IRBranchCondition.LessUnsigned:
                case IRBranchCondition.NotEqualUnsigned:
                    {
                        IRStackObject value2 = pStack.Pop();
                        IRStackObject value1 = pStack.Pop();
						Sources.Add(new IRLinearizedLocation(this, value1.LinearizedTarget));
						Sources.Add(new IRLinearizedLocation(this, value2.LinearizedTarget));
                        break;
                    }
            }
        }

        public override IRInstruction Clone(IRMethod pNewMethod) { return CopyTo(new IRBranchInstruction(BranchCondition, TargetILOffset) { TargetIRInstruction = this.TargetIRInstruction }, pNewMethod); }

		protected override void DumpDetails(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("BranchCondition {0}", BranchCondition);
		}
	}
}
