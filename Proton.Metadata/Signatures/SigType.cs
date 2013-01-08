using System;
using System.Collections.Generic;

namespace Proton.Metadata.Signatures
{
	public sealed class SigType
	{
		public CLIFile CLIFile = null;

		public SigElementType ElementType = SigElementType.End;
		public SigType ArrayType = null;
		public SigArrayShape ArrayShape = null;
		public uint ClassTypeDefOrRefOrSpecToken = 0;
		public MethodSig FnPtrMethodSig = null;
		public bool GenericInstClass = false;
		public bool GenericInstValueType = false;
		public uint GenericInstTypeDefOrRefOrSpecToken = 0;
		public List<SigType> GenericInstGenArgs = null;
		public uint MVarNumber = 0;
		public List<SigCustomMod> PtrMods = null;
		public SigType PtrType = null;
		public bool PtrVoid = false;
		public List<SigCustomMod> SZArrayMods = null;
		public SigType SZArrayType = null;
		public uint ValueTypeDefOrRefOrSpecToken = 0;
		public uint VarNumber = 0;

		public SigType(CLIFile pCLIFile, byte[] pSignature, ref int pCursor)
		{
			CLIFile = pCLIFile;

			ElementType = (SigElementType)pSignature[pCursor++];
			switch (ElementType)
			{
				case SigElementType.Array:
					ArrayType = new SigType(CLIFile, pSignature, ref pCursor);
					ArrayShape = new SigArrayShape(CLIFile, pSignature, ref pCursor);
					break;
				case SigElementType.Class:
					ClassTypeDefOrRefOrSpecToken = CLIFile.ReadCompressedUnsigned(pSignature, ref pCursor);
					break;
				case SigElementType.FunctionPointer:
					FnPtrMethodSig = new MethodSig(CLIFile, pSignature, ref pCursor);
					break;
				case SigElementType.GenericInstantiation:
					{
						GenericInstClass = pSignature[pCursor] == (byte)SigElementType.Class;
						GenericInstValueType = pSignature[pCursor] == (byte)SigElementType.ValueType;
						++pCursor;
						GenericInstTypeDefOrRefOrSpecToken = CLIFile.ReadCompressedUnsigned(pSignature, ref pCursor);
						uint genericInstGenArgCount = CLIFile.ReadCompressedUnsigned(pSignature, ref pCursor);
						GenericInstGenArgs = new List<SigType>((int)genericInstGenArgCount);
						for (uint genericInstGenArgIndex = 0; genericInstGenArgIndex < genericInstGenArgCount; ++genericInstGenArgIndex) GenericInstGenArgs.Add(new SigType(CLIFile, pSignature, ref pCursor));
						break;
					}
				case SigElementType.MethodVar:
					MVarNumber = CLIFile.ReadCompressedUnsigned(pSignature, ref pCursor);
					break;
				case SigElementType.Pointer:
					while (pSignature[pCursor] == (byte)SigElementType.CustomModifier_Required ||
						   pSignature[pCursor] == (byte)SigElementType.CustomModifier_Optional)
					{
						PtrMods.Add(new SigCustomMod(CLIFile, pSignature, ref pCursor));
					}
					if (pSignature[pCursor] == (byte)SigElementType.Void)
					{
						PtrVoid = true;
						++pCursor;
					}
					else PtrType = new SigType(CLIFile, pSignature, ref pCursor);
					break;
				case SigElementType.SingleDimensionArray:
					while (pSignature[pCursor] == (byte)SigElementType.CustomModifier_Required ||
						   pSignature[pCursor] == (byte)SigElementType.CustomModifier_Optional)
					{
						SZArrayMods.Add(new SigCustomMod(CLIFile, pSignature, ref pCursor));
					}
					SZArrayType = new SigType(CLIFile, pSignature, ref pCursor);
					break;
				case SigElementType.ValueType:
					ValueTypeDefOrRefOrSpecToken = CLIFile.ReadCompressedUnsigned(pSignature, ref pCursor);
					break;
				case SigElementType.Var:
					VarNumber = CLIFile.ReadCompressedUnsigned(pSignature, ref pCursor);
					break;

				default: break;
			}
		}
	}
}
