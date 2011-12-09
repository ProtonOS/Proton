#if !LOCALTEST

namespace System {
	public struct Decimal {

#pragma warning disable 169
		// internal representation of decimal
		private uint flags;
		private uint hi;
		private uint lo;
        private uint mid;
#pragma warning restore 169

		public static int[] GetBits(Decimal d) {
			return new int[] { 0, 0, 0, 0 };
		}

	}
}

#endif