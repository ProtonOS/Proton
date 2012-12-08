using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.VM.IR
{
    /// <summary>
    /// Represents a list of type parameters to a generic type.
    /// </summary>
    public sealed class IRGenericParameterList : IEnumerable<IRType>
    {
        /// <summary>
        /// Represents an empty generic parameter list.
        /// </summary>
        public static readonly IRGenericParameterList Empty = new IRGenericParameterList();

        private const int InnerListInitialCapacity = 32;

        private readonly List<IRType> mParams = new List<IRType>(InnerListInitialCapacity);

        /// <summary>
        /// The number of parameters in this list.
        /// </summary>
        public int Count
        {
            get { return mParams.Count; }
        }

        private bool? mResolvedCache;
        /// <summary>
        /// True if all the generic parameters
        /// in this list are resolved.
        /// </summary>
        public bool Resolved
        {
            get
            {
                if (mResolvedCache != null)
                    return mResolvedCache.Value;

                mResolvedCache = mParams.TrueForAll(t => t.Resolved);
                return mResolvedCache.Value;
            }
        }


        public IRGenericParameterList() { }
        public IRGenericParameterList(params IRType[] existingParams) : this((IEnumerable<IRType>)existingParams) { }

        public IRGenericParameterList(IEnumerable<IRType> existingParams)
        {
            foreach (IRType t in existingParams)
            {
                mParams.Add(t);
            }
        }

        /// <summary>
        /// Create a shallow copy of this list.
        /// </summary>
        /// <returns>The new list.</returns>
        public IRGenericParameterList Clone()
        {
            return new IRGenericParameterList(this);
        }

        /// <summary>
        /// Add a Type to this list of generic parameters.
        /// </summary>
        /// <param name="param">The type to add.</param>
        public void Add(IRType param)
        {
            mParams.Add(param);
            if (mResolvedCache != null)
            {
                if (mResolvedCache.Value && !param.Resolved) 
                    mResolvedCache = false;
            }
            mHashCodeCache = null;
        }

        /// <summary>
        /// Add a set of Types to this list of generic parameters.
        /// </summary>
        /// <param name="paramsToAdd">The types to add.</param>
        public void AddRange(IEnumerable<IRType> paramsToAdd)
        {
            mParams.AddRange(paramsToAdd);
            if (mResolvedCache != null)
            {
                if (mResolvedCache.Value)
                {
                    foreach (IRType t in paramsToAdd)
                    {
                        if (!t.Resolved)
                        {
                            mResolvedCache = false;
                            break;
                        }
                    }
                }
            }
            mHashCodeCache = null;
        }

        /// <summary>
        /// Add a set of Types to this list of generic parameters.
        /// </summary>
        /// <param name="paramsToAdd">The types to add.</param>
        public void AddRange(params IRType[] paramsToAdd)
        {
            this.AddRange((IEnumerable<IRType>)paramsToAdd);
        }

        public IRType this[uint idx] { get { return this[(int)idx]; } set { this[(int)idx] = value; } }
        public IRType this[int idx]
        {
            get
            {
                if (idx < 0)
                    throw new IndexOutOfRangeException("A generic parameter index cannot be less than 0!");
                if (idx >= mParams.Count)
                    throw new IndexOutOfRangeException("The requested generic parameter doesn't exist!");
                return mParams[idx];
            }
            set
            {
                if (idx < 0)
                    throw new IndexOutOfRangeException("A generic parameter index cannot be less than 0!");
                if (idx >= mParams.Count)
                    throw new IndexOutOfRangeException("The specified generic parameter doesn't exist!");
                if (mParams[idx] != value)
                {
                    if (mResolvedCache != null)
                    {
                        if (mResolvedCache.Value && !value.Resolved)
                            mResolvedCache = false;
                    }
                    mParams[idx] = value;
                    mHashCodeCache = null;
                }
            }
        }

		public List<IRType> ToList()
		{
			return new List<IRType>(mParams);
		}

        public void Substitute(IRGenericParameterList typeParams, IRGenericParameterList methodParams)
        {
            for (int i = 0; i < mParams.Count; i++)
            {
                IRType t = mParams[i];
                t.Resolve(ref t, typeParams, methodParams);
                mParams[i] = t;
			}
			mHashCodeCache = null;
			mResolvedCache = null;
        }

        public IEnumerator<IRType> GetEnumerator()
        {
            foreach (IRType t in mParams)
                yield return t;
        }

        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            foreach (IRType t in mParams)
                yield return t;
        }

        private int? mHashCodeCache;
        public override int GetHashCode()
        {
            if (mHashCodeCache != null)
                return mHashCodeCache.Value;
            int res = mParams.Count;

            for (int i = 0; i < mParams.Count; i++)
            {
                res ^= mParams[i].GetHashCode();
            }

            mHashCodeCache = res;
            return mHashCodeCache.Value;
        }

        public override bool Equals(object obj)
        {
            if (!(obj is IRGenericParameterList))
                return false;
            return ((IRGenericParameterList)obj).GetHashCode() == this.GetHashCode();
        }

        public static bool operator ==(IRGenericParameterList a, IRGenericParameterList b)
        {
            return a.GetHashCode() == b.GetHashCode();
        }

        public static bool operator !=(IRGenericParameterList a, IRGenericParameterList b)
        {
            return a.GetHashCode() != b.GetHashCode();
        }

		public override string ToString()
		{
			if (mParams.Count == 0)
				return "<>";

			StringBuilder sb = new StringBuilder();
			sb.Append("<");
			mParams.ForEach(t => sb.Append(t.ToString() + ", "));
			sb.Remove(sb.Length - 2, 2);
			sb.Append('>');
			return sb.ToString();
		}
    }
}
