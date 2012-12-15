using System;
using System.Collections;
using System.Collections.Generic;

namespace Proton.VM.IR
{
	public sealed class IRGenericParameterTypeList : IEnumerable<IRType>
	{
		public static readonly IRGenericParameterTypeList Empty = new IRGenericParameterTypeList();

		private const int InnerListInitialCapacity = 8;
		private readonly List<IRType> mTypes = new List<IRType>(InnerListInitialCapacity);
		private int? mHashCodeCache;

        public IRGenericParameterTypeList() { }
		public IRGenericParameterTypeList(IEnumerable<IRType> pTypes) { mTypes.AddRange(pTypes); }

		public int Count { get { return mTypes.Count; } }

		public IRGenericParameterTypeList Copy() { return new IRGenericParameterTypeList(this); }

		public void Add(IRType pType) { mTypes.Add(pType); }

		public void AddRange(IEnumerable<IRType> pTypes) { foreach (IRType type in pTypes) Add(type); }

		public IRType this[uint pIndex] { get { return this[(int)pIndex]; } set { this[(int)pIndex] = value; } }

		public IRType this[int pIndex]
		{
			get
			{
				if (pIndex < 0)
					throw new IndexOutOfRangeException("An IRGenericParameterType index cannot be less than 0!");
				if (pIndex >= mTypes.Count) throw new IndexOutOfRangeException("The requested IRGenericParameterType doesn't exist!");
				return mTypes[pIndex];
			}
			set
			{
				if (pIndex < 0) throw new IndexOutOfRangeException("An IRGenericParameterType index cannot be less than 0!");
				if (pIndex >= mTypes.Count) throw new IndexOutOfRangeException("The specified IRGenericParameterType doesn't exist!");
				if (mTypes[pIndex] != value)
				{
					IRType oldType = mTypes[pIndex];
					IRType newType = value;

					mTypes[pIndex] = value;
				}
			}
		}

		public IEnumerator<IRType> GetEnumerator() { foreach (IRType type in mTypes) yield return type; }

		IEnumerator IEnumerable.GetEnumerator() { foreach (IRType type in mTypes) yield return type; }

		public override int GetHashCode()
		{
			if (mHashCodeCache.HasValue) return mHashCodeCache.Value;
			mHashCodeCache = mTypes.Count;
			mTypes.ForEach(t => mHashCodeCache ^= t.GetHashCode());
			return mHashCodeCache.Value;
		}
	}
}
