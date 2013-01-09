using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Proton.LIR
{
	[StructLayout(LayoutKind.Explicit)]
	public struct LIRImm : ISource
	{
		public bool MayHaveSideEffects { get { return false; } }
		public SourceType SourceType { get { return SourceType.Literal; } }

		[FieldOffset(0)]
		public byte ByteValue;
		[FieldOffset(0)]
		public ushort UShortValue;
		[FieldOffset(0)]
		public uint UIntValue;
		[FieldOffset(0)]
		public ulong ULongValue;
		[FieldOffset(0)]
		public float FloatValue;
		[FieldOffset(0)]
		public double DoubleValue;
		[FieldOffset(8)]
		public LIRValueType ImmType;

		public LIRImm(sbyte val) : this() { ImmType = LIRValueType.Int8; ByteValue = (byte)val; }
		public LIRImm(byte val) : this() { ImmType = LIRValueType.Int8; ByteValue = val; }

		public LIRImm(short val) : this() { ImmType = LIRValueType.Int16; UShortValue = (ushort)val; }
		public LIRImm(ushort val) : this() { ImmType = LIRValueType.Int16; UShortValue = val; }

		public LIRImm(int val) : this() { ImmType = LIRValueType.Int32; UIntValue = (uint)val; }
		public LIRImm(uint val) : this() { ImmType = LIRValueType.Int32; UIntValue = val; }

		public LIRImm(long val) : this() { ImmType = LIRValueType.Int64; ULongValue = (ulong)val; }
		public LIRImm(ulong val) : this() { ImmType = LIRValueType.Int64; ULongValue = val; }

		public LIRImm(float val) : this() { ImmType = LIRValueType.Single; FloatValue = val; }
		public LIRImm(double val) : this() { ImmType = LIRValueType.Double; DoubleValue = val; }


		public static implicit operator LIRImm(sbyte val) { return new LIRImm(val); }
		public static implicit operator LIRImm(byte val) { return new LIRImm(val); }

		public static implicit operator LIRImm(short val) { return new LIRImm(val); }
		public static implicit operator LIRImm(ushort val) { return new LIRImm(val); }

		public static implicit operator LIRImm(int val) { return new LIRImm(val); }
		public static implicit operator LIRImm(uint val) { return new LIRImm(val); }

		public static implicit operator LIRImm(long val) { return new LIRImm(val); }
		public static implicit operator LIRImm(ulong val) { return new LIRImm(val); }

		public static implicit operator LIRImm(float val) { return new LIRImm(val); }
		public static implicit operator LIRImm(double val) { return new LIRImm(val); }

		public override string ToString()
		{
			switch (ImmType)
			{
				case LIRValueType.Int8: return ByteValue.ToString();
				case LIRValueType.Int16: return UShortValue.ToString();
				case LIRValueType.Int32: return UIntValue.ToString();
				case LIRValueType.Int64: return ULongValue.ToString();
				case LIRValueType.Single: return FloatValue.ToString();
				case LIRValueType.Double: return DoubleValue.ToString();
				default: throw new Exception();
			}
		}
	}
}
