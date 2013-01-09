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
			pInstruction.IRIndex = mInstructions.Count;
			mInstructions.Add(pInstruction);
			mResolvedCache = null;
			if (!mILOffsetLookup.ContainsKey(pInstruction.ILOffset))
				mILOffsetLookup.Add(pInstruction.ILOffset, pInstruction);
		}

		public void AddRange(IEnumerable<IRInstruction> pInstructions) { foreach (IRInstruction instruction in pInstructions) Add(instruction); }

		private Dictionary<IRInstruction, IRInstruction> mInsertedTargetFixCache = new Dictionary<IRInstruction, IRInstruction>();
		public void Insert(IRInstruction pInstruction, int pIndex)
		{
			IRInstruction originalInstruction = mInstructions[pIndex];
			pInstruction.IRIndex = pIndex;
			mInstructions.Insert(pIndex, pInstruction);
			for (int index = pIndex + 1; index < mInstructions.Count; ++index) ++mInstructions[index].IRIndex;
			mInsertedTargetFixCache[originalInstruction] = pInstruction;
		}

		private Dictionary<IRInstruction, IRInstruction> mRemovedTargetFixCache = new Dictionary<IRInstruction, IRInstruction>();
		public void Remove(IRInstruction pInstruction)
		{
			mInstructions.RemoveAt(pInstruction.IRIndex);
			for (int index = pInstruction.IRIndex; index < mInstructions.Count; ++index) --mInstructions[index].IRIndex;
			if (pInstruction.IRIndex != mInstructions.Count) mRemovedTargetFixCache[pInstruction] = mInstructions[pInstruction.IRIndex];
		}

		public void FixInsertedTargetInstructions()
		{
			if (mInsertedTargetFixCache.Count > 0)
			{
				IRInstruction targetInstruction = null;
				foreach (IRInstruction instruction in mInstructions)
				{
					switch (instruction.Opcode)
					{
						case IROpcode.Branch:
							{
								IRBranchInstruction branchInstruction = (IRBranchInstruction)instruction;
								if (mInsertedTargetFixCache.TryGetValue(branchInstruction.TargetIRInstruction, out targetInstruction)) branchInstruction.TargetIRInstruction = targetInstruction;
								break;
							}
						case IROpcode.Switch:
							{
								IRSwitchInstruction switchInstruction = (IRSwitchInstruction)instruction;
								for (int index = 0; index < switchInstruction.TargetIRInstructions.Length; ++index)
								{
									if (mInsertedTargetFixCache.TryGetValue(switchInstruction.TargetIRInstructions[index], out targetInstruction))
										switchInstruction.TargetIRInstructions[index] = targetInstruction;
								}
								break;
							}
						case IROpcode.Leave:
							{
								IRLeaveInstruction leaveInstruction = (IRLeaveInstruction)instruction;
								if (mInsertedTargetFixCache.TryGetValue(leaveInstruction.TargetIRInstruction, out targetInstruction)) leaveInstruction.TargetIRInstruction = targetInstruction;
								break;
							}
						default: break;
					}
				}
				mInsertedTargetFixCache.Clear();
			}
		}

		public void FixRemovedTargetInstructions()
		{
			if (mRemovedTargetFixCache.Count > 0)
			{
				IRInstruction targetInstruction = null;
				foreach (IRInstruction instruction in mInstructions)
				{
					switch (instruction.Opcode)
					{
						case IROpcode.Branch:
							{
								IRBranchInstruction branchInstruction = (IRBranchInstruction)instruction;
								if (mRemovedTargetFixCache.TryGetValue(branchInstruction.TargetIRInstruction, out targetInstruction)) branchInstruction.TargetIRInstruction = targetInstruction;
								break;
							}
						case IROpcode.Switch:
							{
								IRSwitchInstruction switchInstruction = (IRSwitchInstruction)instruction;
								for (int index = 0; index < switchInstruction.TargetIRInstructions.Length; ++index)
								{
									if (mRemovedTargetFixCache.TryGetValue(switchInstruction.TargetIRInstructions[index], out targetInstruction))
										switchInstruction.TargetIRInstructions[index] = targetInstruction;
								}
								break;
							}
						case IROpcode.Leave:
							{
								IRLeaveInstruction leaveInstruction = (IRLeaveInstruction)instruction;
								if (mRemovedTargetFixCache.TryGetValue(leaveInstruction.TargetIRInstruction, out targetInstruction)) leaveInstruction.TargetIRInstruction = targetInstruction;
								break;
							}
						default: break;
					}
				}
				mRemovedTargetFixCache.Clear();
			}
		}

		public IRInstruction this[uint pIndex] { get { return this[(int)pIndex]; } set { this[(int)pIndex] = value; } }

		private bool mImmediateRetargetModifiedInstructions = true;
		public bool ImmediateRetargetModifiedInstructions
		{
			get { return mImmediateRetargetModifiedInstructions; }
			set { mImmediateRetargetModifiedInstructions = value; }
		}

		public void FixModifiedTargetInstructions()
		{
			foreach (IRInstruction instruction in mInstructions)
			{
				switch (instruction.Opcode)
				{
					case IROpcode.Branch:
					{
						IRBranchInstruction branchInstruction = (IRBranchInstruction)instruction;
						branchInstruction.TargetIRInstruction = mInstructions[branchInstruction.TargetIRInstruction.IRIndex];
						break;
					}
					case IROpcode.Switch:
					{
						IRSwitchInstruction switchInstruction = (IRSwitchInstruction)instruction;
						for (int index = 0; index < switchInstruction.TargetIRInstructions.Length; ++index)
						{
							switchInstruction.TargetIRInstructions[index] = mInstructions[switchInstruction.TargetIRInstructions[index].IRIndex];
						}
						break;
					}
					case IROpcode.Leave:
					{
						IRLeaveInstruction leaveInstruction = (IRLeaveInstruction)instruction;
						leaveInstruction.TargetIRInstruction = mInstructions[leaveInstruction.TargetIRInstruction.IRIndex];
						break;
					}
					default: break;
				}
			}
		}

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

					if (ImmediateRetargetModifiedInstructions)
					{
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
					else
					{
						value.ParentMethod = this.mInstructions[0].ParentMethod;
						value.IRIndex = pIndex;
					}
				}
			}
		}

		public IEnumerator<IRInstruction> GetEnumerator() { foreach (IRInstruction instruction in mInstructions) yield return instruction; }

		IEnumerator IEnumerable.GetEnumerator() { foreach (IRInstruction instruction in mInstructions) yield return instruction; }

		public void ForEach(Action<IRInstruction> action) { mInstructions.ForEach(action); }

		public bool TrueForAll(Predicate<IRInstruction> match) { return mInstructions.TrueForAll(match); }

		public void RemoveDead()
		{
			if (mInstructions.RemoveAll(i => !i.Linearized) > 0)
			{
				for (int index = 0; index < mInstructions.Count; ++index)
					mInstructions[index].IRIndex = index;
			}
		}

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
							branchInstruction.TargetIRInstruction = mInstructions[branchInstruction.TargetIRInstruction.IRIndex];
							break;
						}
					case IROpcode.Switch:
						{
							IRSwitchInstruction switchInstruction = (IRSwitchInstruction)instruction;
							for (int index = 0; index < switchInstruction.TargetILOffsets.Length; ++index)
							{
								switchInstruction.TargetIRInstructions[index] = mInstructions[switchInstruction.TargetIRInstructions[index].IRIndex];
							}
							break;
						}
					case IROpcode.Leave:
						{
							IRLeaveInstruction leaveInstruction = (IRLeaveInstruction)instruction;
							leaveInstruction.TargetIRInstruction = mInstructions[leaveInstruction.TargetIRInstruction.IRIndex];
							break;
						}
					default: break;
				}
			}
		}
	}
}
