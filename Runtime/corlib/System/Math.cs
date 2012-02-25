using System.Runtime.CompilerServices;

#if LOCALTEST
using System;
namespace System_ {
#else
namespace System {
#endif
	public static class Math {

		public const double E = 2.7182818284590452354;
		public const double PI = 3.14159265358979323846;

        /*
        
	{	"System",			"Math",				"Sin",					Signature_ElementType_R8,		1,	{ Signature_ElementType_R8 }, &System_Math_Sin },
	{	NULL,				NULL,				"Sinh",					Signature_ElementType_R8,		1,	{ Signature_ElementType_R8 }, &System_Math_Sinh },
	{	NULL,				NULL,				"Asin",					Signature_ElementType_R8,		1,	{ Signature_ElementType_R8 }, &System_Math_Asin },
	{	NULL,				NULL,				"Cos",					Signature_ElementType_R8,		1,	{ Signature_ElementType_R8 }, &System_Math_Cos },
	{	NULL,				NULL,				"Cosh",					Signature_ElementType_R8,		1,	{ Signature_ElementType_R8 }, &System_Math_Cosh },
	{	NULL,				NULL,				"Acos",					Signature_ElementType_R8,		1,	{ Signature_ElementType_R8 }, &System_Math_Acos },
	{	NULL,				NULL,				"Tan",					Signature_ElementType_R8,		1,	{ Signature_ElementType_R8 }, &System_Math_Tan },
	{	NULL,				NULL,				"Tanh",					Signature_ElementType_R8,		1,	{ Signature_ElementType_R8 }, &System_Math_Tanh },
	{	NULL,				NULL,				"Atan",					Signature_ElementType_R8,		1,	{ Signature_ElementType_R8 }, &System_Math_Atan },
	{	NULL,				NULL,				"Atan2",				Signature_ElementType_R8,		2,	{ Signature_ElementType_R8, Signature_ElementType_R8 }, &System_Math_Atan2 },
	{	NULL,				NULL,				"Exp",					Signature_ElementType_R8,		1,	{ Signature_ElementType_R8 }, &System_Math_Exp },
	{	NULL,				NULL,				"Pow",					Signature_ElementType_R8,		2,	{ Signature_ElementType_R8, Signature_ElementType_R8 }, &System_Math_Pow },
	{	NULL,				NULL,				"Sqrt",					Signature_ElementType_R8,		1,	{ Signature_ElementType_R8 }, &System_Math_Sqrt },
	{	NULL,				NULL,				"Log",					Signature_ElementType_R8,		2,	{ Signature_ElementType_R8, Signature_ElementType_R8 }, &System_Math_Log },
        */


		#region Abs()

		public static sbyte Abs(sbyte v) {
			if (v == sbyte.MinValue) {
				throw new OverflowException("Value is too small");
			}
			return (v >= 0) ? v : (sbyte)-v;
		}

		public static short Abs(short v) {
			if (v == short.MinValue) {
				throw new OverflowException("Value is too small");
			}
			return (v >= 0) ? v : (short)-v;
		}

		public static int Abs(int v) {
			if (v == int.MinValue) {
				throw new OverflowException("Value is too small");
			}
			return (v >= 0) ? v : -v;
		}

		public static long Abs(long v) {
			if (v == long.MinValue) {
				throw new OverflowException("Value is too small");
			}
			return (v >= 0) ? v : -v;
		}

		public static float Abs(float v) {
			return (v >= 0) ? v : -v;
		}

		public static double Abs(double v) {
			return (v >= 0) ? v : -v;
		}

		#endregion

		#region Min()

		public static sbyte Min(sbyte v1, sbyte v2) {
			return (v1 < v2) ? v1 : v2;
		}

		public static short Min(short v1, short v2) {
			return (v1 < v2) ? v1 : v2;
		}

		public static int Min(int v1, int v2) {
			return (v1 < v2) ? v1 : v2;
		}

		public static long Min(long v1, long v2) {
			return (v1 < v2) ? v1 : v2;
		}

		public static byte Min(byte v1, byte v2) {
			return (v1 < v2) ? v1 : v2;
		}

		public static ushort Min(ushort v1, ushort v2) {
			return (v1 < v2) ? v1 : v2;
		}

		public static uint Min(uint v1, uint v2) {
			return (v1 < v2) ? v1 : v2;
		}

		public static ulong Min(ulong v1, ulong v2) {
			return (v1 < v2) ? v1 : v2;
		}

		public static float Min(float v1, float v2) {
			if (float.IsNaN(v1) || float.IsNaN(v2)) {
				return float.NaN;
			}
			return (v1 < v2) ? v1 : v2;
		}

		public static double Min(double v1, double v2) {
			if (double.IsNaN(v1) || double.IsNaN(v2)) {
				return double.NaN;
			}
			return (v1 < v2) ? v1 : v2;
		}

		#endregion

		#region Max()

		public static sbyte Max(sbyte v1, sbyte v2) {
			return (v1 > v2) ? v1 : v2;
		}

		public static short Max(short v1, short v2) {
			return (v1 > v2) ? v1 : v2;
		}

		public static int Max(int v1, int v2) {
			return (v1 > v2) ? v1 : v2;
		}

		public static long Max(long v1, long v2) {
			return (v1 > v2) ? v1 : v2;
		}

		public static byte Max(byte v1, byte v2) {
			return (v1 > v2) ? v1 : v2;
		}

		public static ushort Max(ushort v1, ushort v2) {
			return (v1 > v2) ? v1 : v2;
		}

		public static uint Max(uint v1, uint v2) {
			return (v1 > v2) ? v1 : v2;
		}

		public static ulong Max(ulong v1, ulong v2) {
			return (v1 > v2) ? v1 : v2;
		}

		public static float Max(float v1, float v2) {
			if (float.IsNaN(v1) || float.IsNaN(v2)) {
				return float.NaN;
			}
			return (v1 > v2) ? v1 : v2;
		}

		public static double Max(double v1, double v2) {
			if (double.IsNaN(v1) || double.IsNaN(v2)) {
				return double.NaN;
			}
			return (v1 > v2) ? v1 : v2;
		}

		#endregion

		#region Sign()

		public static int Sign(sbyte v) {
			return (v > 0) ? 1 : ((v < 0) ? -1 : 0);
		}

		public static int Sign(short v) {
			return (v > 0) ? 1 : ((v < 0) ? -1 : 0);
		}

		public static int Sign(int v) {
			return (v > 0) ? 1 : ((v < 0) ? -1 : 0);
		}

		public static int Sign(long v) {
			return (v > 0) ? 1 : ((v < 0) ? -1 : 0);
		}

		public static int Sign(float v) {
			if (float.IsNaN(v)) {
				throw new ArithmeticException("NaN");
			}
			return (v > 0) ? 1 : ((v < 0) ? -1 : 0);
		}

		public static int Sign(double v) {
			if (double.IsNaN(v)) {
				throw new ArithmeticException("NaN");
			}
			return (v > 0) ? 1 : ((v < 0) ? -1 : 0);
		}

		#endregion

        public static double Ceiling(double x)
        {
            if (x == double.NaN || x == double.NegativeInfinity || x == double.PositiveInfinity) return x;
            return (x - (long)x > 0) ? (long)(x + 1) : (long)x;
        }

        public static double Floor(double x)
        {
            if (x == 0.0) return 0.0;
            if (x == double.NaN || x == double.NegativeInfinity || x == double.PositiveInfinity) return x;
            return (long)x;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern static double Sin(double x);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static double Sinh(double x);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static double Asin(double x);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern static double Cos(double x);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static double Cosh(double x);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static double Acos(double x);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern static double Tan(double x);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static double Tanh(double x);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static double Atan(double x);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static double Atan2(double x, double y);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static double Exp(double x);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern static double Pow(double x, double y);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern static double Sqrt(double x);

        public static double Log(double x) { return Log(x, E); }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static double Log(double x, double y);

        public static double Log10(double x) { return Log(x, 10.0); }

        public static double Round(double x)
        {
            double floor = Floor(x);
            return (floor % 2 == 0) ? floor : Ceiling(x);
        }

        public static double Truncate(double x) { return Floor(x); }
    }
}
