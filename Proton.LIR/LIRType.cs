using System;
using System.Collections.Generic;

namespace Proton.LIR
{
	public enum LIRValueType : int
	{
		None = 0,
		Int8,
		Int16,
		Int32,
		Int64,
		Single,
		Double,
	}

	public sealed class LIRType
	{
		/// <summary>
		/// The size of this type in bytes.
		/// </summary>
		public uint Size { get; private set; }
		public bool Signed { get; private set; }
		public LIRValueType Type { get; private set; }
#warning Eventually need to create constructors which take allocatable as part of their parameters and make the set on this private.
		public bool Allocatable { get; set; }

		public LIRType(uint size)
		{
			this.Type = LIRValueType.None;
			this.Size = size;
			this.Signed = false;
		}

		public LIRType(uint size, bool signed, bool floating = false)
		{
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

		public LIRType(LIRValueType tp, bool signed = false)
		{
			this.Type = tp;
			this.Signed = signed;
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
		
	}
}
