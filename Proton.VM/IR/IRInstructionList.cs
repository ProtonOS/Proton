using Proton.VM.IR.Instructions;
using System;
using System.Collections;
using System.Collections.Generic;

namespace Proton.VM.IR
{
    public sealed class IRInstructionList : IEnumerable<IRInstruction>
    {
        public static readonly IRInstructionList Empty = new IRInstructionList();

        private const int InnerListInitialCapacity = 32;

        private readonly List<IRInstruction> mInstructions = new List<IRInstruction>(InnerListInitialCapacity);
        private readonly Dictionary<int, IRInstruction> mILOffsetLookup = new Dictionary<int, IRInstruction>();

        public int Count { get { return mInstructions.Count; } }

        private bool? mResolvedCache;

        public bool Resolved
        {
            get
            {
                if (mResolvedCache.HasValue) return mResolvedCache.Value;
                mResolvedCache = mInstructions.TrueForAll(i => i.Resolved);
                return mResolvedCache.Value;
            }
        }

        public IRInstructionList() { }
        public IRInstructionList(IEnumerable<IRInstruction> pInstructions) { mInstructions.AddRange(pInstructions); }

        public IRInstructionList Clone() { return new IRInstructionList(this); }

        public void Add(IRInstruction pInstruction)
        {
            mInstructions.Add(pInstruction);
            mResolvedCache = null;
            mILOffsetLookup.Add(pInstruction.ILOffset, pInstruction);
        }

        public void AddRange(IEnumerable<IRInstruction> pInstructions) { foreach (IRInstruction instruction in pInstructions) Add(instruction); }

		public void Insert(IRInstruction pInstruction, uint pIndex)
		{
			pInstruction.IRIndex = pIndex;
			mInstructions.Insert((int)pIndex, pInstruction);
			for (int index = (int)(pIndex + 1); index < mInstructions.Count; ++index) ++mInstructions[index].IRIndex;
		}

		public IRInstruction this[uint pIndex] { get { return this[(int)pIndex]; } set { this[(int)pIndex] = value; } }

        public IRInstruction this[int pIndex]
        {
            get
            {
                if (pIndex < 0)
                    throw new IndexOutOfRangeException("An IRInstruction index cannot be less than 0!");
                if (pIndex >= mInstructions.Count) throw new IndexOutOfRangeException("The requested IRInstruction doesn't exist!");
                return mInstructions[pIndex];
            }
            set
            {
                if (pIndex < 0) throw new IndexOutOfRangeException("An IRInstruction index cannot be less than 0!");
                if (pIndex >= mInstructions.Count) throw new IndexOutOfRangeException("The specified IRInstruction doesn't exist!");
                if (mInstructions[pIndex] != value)
                {
                    if (!value.Resolved) mResolvedCache = false;
                    IRInstruction oldInstruction = mInstructions[pIndex];
                    IRInstruction newInstruction = value;

                    mInstructions[pIndex] = value;
                    mILOffsetLookup[newInstruction.ILOffset] = newInstruction;
                    foreach (IRInstruction instruction in mInstructions)
                    {
                        switch (instruction.Opcode)
                        {
                            case IROpcode.Branch:
                                {
                                    IRBranchInstruction branchInstruction = (IRBranchInstruction)instruction;
                                    if (branchInstruction.TargetIRInstruction == oldInstruction) branchInstruction.TargetIRInstruction = newInstruction;
                                    break;
                                }
                            case IROpcode.Switch:
                                {
                                    IRSwitchInstruction switchInstruction = (IRSwitchInstruction)instruction;
                                    for (int index = 0; index < switchInstruction.TargetIRInstructions.Length; ++index)
                                    {
                                        if (switchInstruction.TargetIRInstructions[index] == oldInstruction) switchInstruction.TargetIRInstructions[index] = newInstruction;
                                    }
                                    break;
                                }
                            case IROpcode.Leave:
                                {
                                    IRLeaveInstruction leaveInstruction = (IRLeaveInstruction)instruction;
                                    if (leaveInstruction.TargetIRInstruction == oldInstruction) leaveInstruction.TargetIRInstruction = newInstruction;
                                    break;
                                }
                            default: break;
                        }
                    }
                }
            }
        }

        public IEnumerator<IRInstruction> GetEnumerator() { foreach (IRInstruction instruction in mInstructions) yield return instruction; }

        IEnumerator IEnumerable.GetEnumerator() { foreach (IRInstruction instruction in mInstructions) yield return instruction; }

        public void ForEach(Action<IRInstruction> action) { mInstructions.ForEach(action); }

        public bool TrueForAll(Predicate<IRInstruction> match) { return mInstructions.TrueForAll(match); }

        private IRInstruction GetTarget(IRBranchInstruction pBranchInstruction)
        {
            IRInstruction targetInstruction = mILOffsetLookup[pBranchInstruction.TargetILOffset];
            if (targetInstruction.Opcode == IROpcode.Branch)
            {
                IRBranchInstruction targetBranchInstruction = (IRBranchInstruction)targetInstruction;
                if (targetBranchInstruction.BranchCondition == IRBranchCondition.Always)
                {
                    targetInstruction = mILOffsetLookup[targetBranchInstruction.TargetILOffset];
                    targetInstruction = GetTarget(targetBranchInstruction);
                }
            }
            return targetInstruction;
        }

        public void LoadTargetInstructions()
        {
            foreach (IRInstruction instruction in mInstructions)
            {
                switch (instruction.Opcode)
                {
                    case IROpcode.Branch:
                        {
                            IRBranchInstruction branchInstruction = (IRBranchInstruction)instruction;
                            branchInstruction.TargetIRInstruction = GetTarget(branchInstruction);
                            if (branchInstruction.TargetIRInstruction == null) throw new NullReferenceException();
                            break;
                        }
                    case IROpcode.Switch:
                        {
                            IRSwitchInstruction switchInstruction = (IRSwitchInstruction)instruction;
                            switchInstruction.TargetIRInstructions = new IRInstruction[switchInstruction.TargetILOffsets.Length];
                            for (int index = 0; index < switchInstruction.TargetILOffsets.Length; ++index)
                            {
                                switchInstruction.TargetIRInstructions[index] = mILOffsetLookup[switchInstruction.TargetILOffsets[index]];
                                if (switchInstruction.TargetIRInstructions[index] == null) throw new NullReferenceException();
                            }
                            break;
                        }
                    case IROpcode.Leave:
                        {
                            IRLeaveInstruction leaveInstruction = (IRLeaveInstruction)instruction;
                            leaveInstruction.TargetIRInstruction = mILOffsetLookup[leaveInstruction.TargetILOffset];
                            if (leaveInstruction.TargetIRInstruction == null) throw new NullReferenceException();
                            break;
                        }
                    default: break;
                }
            }
        }

		public void FixClonedTargetInstructions()
		{
			foreach (IRInstruction instruction in mInstructions)
			{
				switch (instruction.Opcode)
				{
					case IROpcode.Branch:
						{
							IRBranchInstruction branchInstruction = (IRBranchInstruction)instruction;
							branchInstruction.TargetIRInstruction = mInstructions[(int)branchInstruction.TargetIRInstruction.IRIndex];
							break;
						}
					case IROpcode.Switch:
						{
							IRSwitchInstruction switchInstruction = (IRSwitchInstruction)instruction;
							for (int index = 0; index < switchInstruction.TargetILOffsets.Length; ++index)
							{
								switchInstruction.TargetIRInstructions[index] = mInstructions[(int)switchInstruction.TargetIRInstructions[index].IRIndex];
							}
							break;
						}
					case IROpcode.Leave:
						{
							IRLeaveInstruction leaveInstruction = (IRLeaveInstruction)instruction;
							leaveInstruction.TargetIRInstruction = mInstructions[(int)leaveInstruction.TargetIRInstruction.IRIndex];
							break;
						}
					default: break;
				}
			}
		}
    }
}
