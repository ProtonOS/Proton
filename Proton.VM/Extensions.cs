using System;
using System.Collections.Generic;

namespace Proton.VM
{
	public static class Extensions
	{
		public static T Last<T>(this T[] pThis)
		{
			return pThis[pThis.Length - 1];
		}

		public static T Last<T>(this List<T> pThis)
		{
			return pThis[pThis.Count - 1];
		}

		public static bool TrueForAll<T>(this IEnumerable<T> pThis, Func<T, bool> fc)
		{
			foreach (T t in pThis) if (!fc(t)) return false;
			return true;
		}

		public static void ForEach<T>(this IEnumerable<T> pThis, Action<T> action)
		{
			foreach (T t in pThis) action(t);
		}

		public static Stack<T> Duplicate<T>(this Stack<T> pThis)
		{
			T[] elements = pThis.ToArray();
			Array.Reverse(elements);
			return new Stack<T>(elements);
		}

		public static bool Exists<T>(this IEnumerable<T> pThis, Func<T, bool> fc)
		{
			foreach (T t in pThis) if (fc(t)) return true;
			return false;
		}

		public static int PointerAlign(this int pThis)
		{
			int remainder = pThis & ((VMConfig.PointerSizeForTarget << 3) - 1);
			if (remainder > 0) remainder = VMConfig.PointerSizeForTarget - remainder;
			return pThis + remainder;
		}

		public static int WordAlign(this int pThis)
		{
			int remainder = pThis & ((VMConfig.WordSizeForTarget << 3) - 1);
			if (remainder > 0) remainder = VMConfig.WordSizeForTarget - remainder;
			return pThis + remainder;
		}
	}
}
