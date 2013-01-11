using System;
using System.Collections.Generic;

namespace Proton.LIR
{
	public enum LIRValueType : byte
	{
		None = 0,
		Int8,
		Int16,
		Int32,
		Int64,
		Single,
		Double,

		// This should never have more than
		// 32 elements otherwise it will break
		// hashcode creation. (6-bits at most)
	}

	public sealed class LIRType
	{
		/// <summary>
		/// The size of this type in bytes. (should never have a bit above the 24th bit set)
		/// </summary>
		public uint Size { get; private set; }
		public LIRValueType Type { get; private set; }
		public bool Signed { get; private set; }
		public bool Allocatable { get; set; }

		private static Dictionary<int, LIRType> CreatedLIRTypes = new Dictionary<int, LIRType>();
		private static LIRType LocateLIRType(LIRType t)
		{
			LIRType f;
			if (CreatedLIRTypes.TryGetValue(t.GetHashCode(), out f))
				return f;
			CreatedLIRTypes.Add(t.GetHashCode(), t);
			return t;
		}

		private LIRType(uint size)
		{
			this.Allocatable = true;
			this.Type = LIRValueType.None;
			this.Size = size;
			this.Signed = false;
		}
		public static LIRType GetLIRType(uint size, bool allocatable) { return LocateLIRType(new LIRType(size) { Allocatable = allocatable }); }

		private LIRType(uint size, bool signed, bool floating = false)
		{
			this.Allocatable = true;
			switch (size)
			{
				case 1:
					if (floating)
						throw new NotSupportedException("Mini precision floating point values are not supported!");
					else
						Type = LIRValueType.Int8;
					break;
				case 2:
					if (floating)
						throw new NotSupportedException("Half precision floating point values are not supported!");
					else
						Type = LIRValueType.Int16;
					break;
				case 4:
					if (floating)
						Type = LIRValueType.Single;
					else
						Type = LIRValueType.Int32;
					break;
				case 8:
					if (floating)
						Type = LIRValueType.Double;
					else
						Type = LIRValueType.Int64;
					break;
				default:
					throw new ArgumentOutOfRangeException("size");
			}
			this.Size = size;
			if (floating)
			{
				if (!signed)
					throw new ArgumentException("Tisk tisk, you should know that a floating point value can never be unsigned!");
				this.Signed = true;
			}
			else
				this.Signed = signed;
		}
		public static LIRType GetLIRType(uint size, bool signed, bool floating, bool allocatable = true) { return LocateLIRType(new LIRType(size, signed, floating) { Allocatable = allocatable }); }

		private LIRType(LIRValueType tp, bool signed = false)
		{
			this.Type = tp;
			this.Signed = signed;
			this.Allocatable = true;
			switch (tp)
			{
				case LIRValueType.Int8:
					this.Size = 1;
					break;
				case LIRValueType.Int16:
					this.Size = 2;
					break;
				case LIRValueType.Int32:
					this.Size = 4;
					break;
				case LIRValueType.Int64:
					this.Size = 8;
					break;
				case LIRValueType.Single:
					this.Size = 4;
					this.Signed = true;
					break;
				case LIRValueType.Double:
					this.Size = 8;
					this.Signed = true;
					break;

				case LIRValueType.None:
				default:
					throw new ArgumentOutOfRangeException("tp");
			}
		}
		public static LIRType GetLIRType(LIRValueType tp, bool signed = false) { return LocateLIRType(new LIRType(tp, signed)); }

		public override string ToString()
		{
			return String.Format("{0}{1}:{2}{3}", Type, (!Allocatable ? "&" : ""), (Signed ? "@" : ""), Size);
		}

		public override int GetHashCode()
		{
			return (int)((Size & 0x00FFFFFF) << 8) | (int)((Signed ? 1 : 0) << 8) | (int)((Allocatable ? 1 : 0) << 7) | (int)((byte)Type & 0x3F);
		}
		
	}
}
