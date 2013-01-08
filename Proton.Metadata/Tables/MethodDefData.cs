using Proton.Metadata.Signatures;
using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
	public sealed class MethodDefData
	{
		public static void Initialize(CLIFile pFile)
		{
			if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.MethodDef)) != 0)
			{
				pFile.MethodDefTable = new MethodDefData[pFile.ReadInt32()];
				for (int index = 0; index < pFile.MethodDefTable.Length; ++index) pFile.MethodDefTable[index] = new MethodDefData() { CLIFile = pFile, TableIndex = index };
			}
			else pFile.MethodDefTable = new MethodDefData[0];
		}

		public static void Load(CLIFile pFile)
		{
			for (int index = 0; index < pFile.MethodDefTable.Length; ++index) pFile.MethodDefTable[index].LoadData(pFile);
		}

		public static void Link(CLIFile pFile)
		{
			for (int index = 0; index < pFile.MethodDefTable.Length; ++index) pFile.MethodDefTable[index].LinkData(pFile);
		}

		public CLIFile CLIFile = null;

		public int TableIndex = 0;
		public uint RVA = 0;
		public MethodImplAttributes ImplFlags = 0;
		public MethodAttributes Flags = MethodAttributes.None;
		public string Name = null;
		public byte[] Signature = null;
		public int ParamListIndex = 0;
		public List<ParamData> ParamList = new List<ParamData>();

		public TypeDefData ParentTypeDef = null;
		public MethodDefBodyData Body = null;
		public MethodSig ExpandedSignature = null;

		private void LoadData(CLIFile pFile)
		{
			RVA = pFile.ReadUInt32();
			ImplFlags = (MethodImplAttributes)pFile.ReadUInt16();
			Flags = (MethodAttributes)pFile.ReadUInt16();
			Name = pFile.ReadStringHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Strings32Bit));
			Signature = pFile.ReadBlobHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Blob32Bit));
			if (pFile.ParamTable.Length >= 0xFFFF) ParamListIndex = pFile.ReadInt32() - 1;
			else ParamListIndex = pFile.ReadUInt16() - 1;

			if (RVA != 0)
			{
				Body = new MethodDefBodyData();
				Body.LoadData(this);
			}
		}

		private void LinkData(CLIFile pFile)
		{
			int paramListCount = pFile.ParamTable.Length - ParamListIndex;
			if (TableIndex < (pFile.MethodDefTable.Length - 1)) paramListCount = pFile.MethodDefTable[TableIndex + 1].ParamListIndex - ParamListIndex;
			for (int index = 0; index < paramListCount; ++index) { ParamList.Add(pFile.ParamTable[ParamListIndex + index]); pFile.ParamTable[ParamListIndex + index].ParentMethodDef = this; }

			int cursor = 0;
			ExpandedSignature = new MethodSig(pFile, Signature, ref cursor);
		}

		public sealed class MethodDefBodyData
		{
			public sealed class MethodDefBodyExceptionData
			{
				public uint Flags = 0;
				public uint TryOffset = 0;
				public uint TryLength = 0;
				public uint HandlerOffset = 0;
				public uint HandlerLength = 0;
				public uint ClassTokenOrFilterOffset = 0;
			}

			public ushort Flags = 0;
			public ushort MaxStack = 0;
			public uint CodeSize = 0;
			public uint LocalVarSigToken = 0;
			public uint CodeRVA = 0;
			public MethodDefBodyExceptionData[] Exceptions = null;

			public StandAloneSigData LocalVarSignature = null;
			public LocalVarSig ExpandedLocalVarSignature = null;

			public void LoadData(MethodDefData pMethodDef)
			{
				CLIFile.SectionHeader sectionHeader = pMethodDef.CLIFile.GetSection(pMethodDef.RVA);
				uint startOfBodyHeader = sectionHeader.PointerToRawData + (pMethodDef.RVA - sectionHeader.VirtualAddress);
				uint sizeOfBodyHeader = 1;
				Flags = (ushort)(pMethodDef.CLIFile.Data[startOfBodyHeader] & 0x03);
				MaxStack = 8;
				CodeSize = (uint)(pMethodDef.CLIFile.Data[startOfBodyHeader] >> 2);
				if ((Flags & 0x03) == 0x03)
				{
					Flags = pMethodDef.CLIFile.Data[startOfBodyHeader];
					Flags |= (ushort)((pMethodDef.CLIFile.Data[startOfBodyHeader + 1] & 0x0F) << 8);
					sizeOfBodyHeader = (uint)((pMethodDef.CLIFile.Data[startOfBodyHeader + 1] >> 4) * 4);
					MaxStack = pMethodDef.CLIFile.Data[startOfBodyHeader + 2];
					MaxStack |= (ushort)(pMethodDef.CLIFile.Data[startOfBodyHeader + 3] << 8);
					CodeSize = pMethodDef.CLIFile.Data[startOfBodyHeader + 4];
					CodeSize |= (uint)(pMethodDef.CLIFile.Data[startOfBodyHeader + 5] << 8);
					CodeSize |= (uint)(pMethodDef.CLIFile.Data[startOfBodyHeader + 6] << 16);
					CodeSize |= (uint)(pMethodDef.CLIFile.Data[startOfBodyHeader + 7] << 24);
					LocalVarSigToken = pMethodDef.CLIFile.Data[startOfBodyHeader + 8];
					LocalVarSigToken |= (uint)(pMethodDef.CLIFile.Data[startOfBodyHeader + 9] << 8);
					LocalVarSigToken |= (uint)(pMethodDef.CLIFile.Data[startOfBodyHeader + 10] << 16);
					LocalVarSigToken |= (uint)(pMethodDef.CLIFile.Data[startOfBodyHeader + 11] << 24);
				}
				CodeRVA = startOfBodyHeader + sizeOfBodyHeader;
				List<MethodDefBodyExceptionData> exceptions = new List<MethodDefBodyExceptionData>();
				if ((Flags & 0x08) == 0x08)
				{
					uint startOfExceptionTableHeader = CodeRVA + CodeSize;
					if ((startOfExceptionTableHeader & 0x03) != 0) startOfExceptionTableHeader += 4 - (startOfExceptionTableHeader & 0x03);
					uint startOfCurrentExceptionTableHeader = startOfExceptionTableHeader;
					bool anotherSection = true;
					while (anotherSection)
					{
						bool fatEntries = (pMethodDef.CLIFile.Data[startOfCurrentExceptionTableHeader] & 0x40) == 0x40;
						anotherSection = (pMethodDef.CLIFile.Data[startOfCurrentExceptionTableHeader] & 0x80) == 0x80;
						uint totalSize = pMethodDef.CLIFile.Data[startOfCurrentExceptionTableHeader + 1];
						uint entrySize = 12;
						if (fatEntries)
						{
							totalSize |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionTableHeader + 2] << 8);
							totalSize |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionTableHeader + 3] << 16);
							entrySize = 24;
						}
						uint entryCount = (uint)((totalSize - 4) / entrySize);
						uint startOfCurrentExceptionEntry = startOfCurrentExceptionTableHeader + 4;
						for (int index = 0; index < entryCount; ++index, startOfCurrentExceptionEntry += entrySize)
						{
							MethodDefBodyExceptionData exception = new MethodDefBodyExceptionData();
							exception.Flags = pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry];
							exception.Flags |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 1] << 8);
							exception.TryOffset = pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 2];
							exception.TryOffset |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 3] << 8);
							exception.TryLength = pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 4];
							exception.HandlerOffset = pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 5];
							exception.HandlerOffset |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 6] << 8);
							exception.HandlerLength = pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 7];
							exception.ClassTokenOrFilterOffset = pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 8];
							exception.ClassTokenOrFilterOffset |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 9] << 8);
							exception.ClassTokenOrFilterOffset |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 10] << 16);
							exception.ClassTokenOrFilterOffset |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 11] << 24);
							if (fatEntries)
							{
								exception.Flags = pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry];
								exception.Flags |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 1] << 8);
								exception.Flags |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 2] << 16);
								exception.Flags |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 3] << 24);
								exception.TryOffset = pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 4];
								exception.TryOffset |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 5] << 8);
								exception.TryOffset |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 6] << 16);
								exception.TryOffset |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 7] << 24);
								exception.TryLength = pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 8];
								exception.TryLength |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 9] << 8);
								exception.TryLength |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 10] << 16);
								exception.TryLength |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 11] << 24);
								exception.HandlerOffset = pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 12];
								exception.HandlerOffset |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 13] << 8);
								exception.HandlerOffset |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 14] << 16);
								exception.HandlerOffset |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 15] << 24);
								exception.HandlerLength = pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 16];
								exception.HandlerLength |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 17] << 8);
								exception.HandlerLength |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 18] << 16);
								exception.HandlerLength |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 19] << 24);
								exception.ClassTokenOrFilterOffset = pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 20];
								exception.ClassTokenOrFilterOffset |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 21] << 8);
								exception.ClassTokenOrFilterOffset |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 22] << 16);
								exception.ClassTokenOrFilterOffset |= (uint)(pMethodDef.CLIFile.Data[startOfCurrentExceptionEntry + 23] << 24);
							}
							exceptions.Add(exception);
						}

						startOfCurrentExceptionTableHeader += totalSize;
					}
				}
				Exceptions = exceptions.ToArray();
			}

			public void LinkData(MethodDefData pMethodDef)
			{
				if (LocalVarSigToken != 0)
				{
					LocalVarSignature = (StandAloneSigData)pMethodDef.CLIFile.ExpandMetadataToken(LocalVarSigToken).Data;
					ExpandedLocalVarSignature = LocalVarSignature.ExpandedLocalVarSignature;
				}
			}
		}
	}
}
