using Proton.Metadata.Tables;
using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata
{
	public sealed class CLIFile
	{
		private delegate void CLIFileDelegate(CLIFile pFile);
		private class MetadataLoader
		{
			public CLIFileDelegate Initializer;
			public CLIFileDelegate Loader;
			public CLIFileDelegate Linker;
			public MetadataLoader(CLIFileDelegate pInitializer, CLIFileDelegate pLoader, CLIFileDelegate pLinker) { Initializer = pInitializer; Loader = pLoader; Linker = pLinker; }
		}
		private static MetadataLoader[] sMetadataLoaders = new MetadataLoader[]
        {
            new MetadataLoader(ModuleData.Initialize, ModuleData.Load, ModuleData.Link),
            new MetadataLoader(TypeRefData.Initialize, TypeRefData.Load, TypeRefData.Link),
            new MetadataLoader(TypeDefData.Initialize, TypeDefData.Load, TypeDefData.Link),
            new MetadataLoader(FieldData.Initialize, FieldData.Load, FieldData.Link),
            new MetadataLoader(MethodDefData.Initialize, MethodDefData.Load, MethodDefData.Link),
            new MetadataLoader(ParamData.Initialize, ParamData.Load, ParamData.Link),
            new MetadataLoader(InterfaceImplData.Initialize, InterfaceImplData.Load, InterfaceImplData.Link),
            new MetadataLoader(MemberRefData.Initialize, MemberRefData.Load, MemberRefData.Link),
            new MetadataLoader(ConstantData.Initialize, ConstantData.Load, ConstantData.Link),
            new MetadataLoader(CustomAttributeData.Initialize, CustomAttributeData.Load, CustomAttributeData.Link),
            new MetadataLoader(FieldMarshalData.Initialize, FieldMarshalData.Load, FieldMarshalData.Link),
            new MetadataLoader(DeclSecurityData.Initialize, DeclSecurityData.Load, DeclSecurityData.Link),
            new MetadataLoader(ClassLayoutData.Initialize, ClassLayoutData.Load, ClassLayoutData.Link),
            new MetadataLoader(FieldLayoutData.Initialize, FieldLayoutData.Load, FieldLayoutData.Link),
            new MetadataLoader(StandAloneSigData.Initialize, StandAloneSigData.Load, StandAloneSigData.Link),
            new MetadataLoader(EventMapData.Initialize, EventMapData.Load, EventMapData.Link),
            new MetadataLoader(EventData.Initialize, EventData.Load, EventData.Link),
            new MetadataLoader(PropertyMapData.Initialize, PropertyMapData.Load, PropertyMapData.Link),
            new MetadataLoader(PropertyData.Initialize, PropertyData.Load, PropertyData.Link),
            new MetadataLoader(MethodSemanticsData.Initialize, MethodSemanticsData.Load, MethodSemanticsData.Link),
            new MetadataLoader(MethodImplData.Initialize, MethodImplData.Load, MethodImplData.Link),
            new MetadataLoader(ModuleRefData.Initialize, ModuleRefData.Load, ModuleRefData.Link),
            new MetadataLoader(TypeSpecData.Initialize, TypeSpecData.Load, TypeSpecData.Link),
            new MetadataLoader(ImplMapData.Initialize, ImplMapData.Load, ImplMapData.Link),
            new MetadataLoader(FieldRVAData.Initialize, FieldRVAData.Load, FieldRVAData.Link),
            new MetadataLoader(AssemblyData.Initialize, AssemblyData.Load, AssemblyData.Link),
            new MetadataLoader(AssemblyProcessorData.Initialize, AssemblyProcessorData.Load, AssemblyProcessorData.Link),
            new MetadataLoader(AssemblyOSData.Initialize, AssemblyOSData.Load, AssemblyOSData.Link),
            new MetadataLoader(AssemblyRefData.Initialize, AssemblyRefData.Load, AssemblyRefData.Link),
            new MetadataLoader(AssemblyRefProcessorData.Initialize, AssemblyRefProcessorData.Load, AssemblyRefProcessorData.Link),
            new MetadataLoader(AssemblyRefOSData.Initialize, AssemblyRefOSData.Load, AssemblyRefOSData.Link),
            new MetadataLoader(FileData.Initialize, FileData.Load, FileData.Link),
            new MetadataLoader(ExportedTypeData.Initialize, ExportedTypeData.Load, ExportedTypeData.Link),
            new MetadataLoader(ManifestResourceData.Initialize, ManifestResourceData.Load, ManifestResourceData.Link),
            new MetadataLoader(NestedClassData.Initialize, NestedClassData.Load, NestedClassData.Link),
            new MetadataLoader(GenericParamData.Initialize, GenericParamData.Load, GenericParamData.Link),
            new MetadataLoader(MethodSpecData.Initialize, MethodSpecData.Load, MethodSpecData.Link),
            new MetadataLoader(GenericParamConstraintData.Initialize, GenericParamConstraintData.Load, GenericParamConstraintData.Link)
        };

		public string ReferenceName = null;
		private byte[] mData = null;
		private int mCursor = 0;
		public DOSHeader DOS = new DOSHeader();
		public PEHeader PE = new PEHeader();
		public OptionalHeader Optional = new OptionalHeader();
		public SectionHeader[] Sections = null;
		public CLIHeader CLI = new CLIHeader();
		public uint CLIMetadataHeaderOffset = 0;
		public CLIMetadataHeader CLIMetadata = new CLIMetadataHeader();
		public CLIMetadataStreamHeader Tables = null;
		public CLIMetadataStreamHeader Strings = null;
		public CLIMetadataStreamHeader US = null;
		public CLIMetadataStreamHeader GUID = null;
		public CLIMetadataStreamHeader Blob = null;
		public CLIMetadataTablesHeader CLIMetadataTables = new CLIMetadataTablesHeader();

		public ModuleData[] ModuleTable = null;
		public TypeRefData[] TypeRefTable = null;
		public TypeDefData[] TypeDefTable = null;
		public FieldData[] FieldTable = null;
		public MethodDefData[] MethodDefTable = null;
		public ParamData[] ParamTable = null;
		public InterfaceImplData[] InterfaceImplTable = null;
		public MemberRefData[] MemberRefTable = null;
		public ConstantData[] ConstantTable = null;
		public CustomAttributeData[] CustomAttributeTable = null;
		public FieldMarshalData[] FieldMarshalTable = null;
		public DeclSecurityData[] DeclSecurityTable = null;
		public ClassLayoutData[] ClassLayoutTable = null;
		public FieldLayoutData[] FieldLayoutTable = null;
		public StandAloneSigData[] StandAloneSigTable = null;
		public EventMapData[] EventMapTable = null;
		public EventData[] EventTable = null;
		public PropertyMapData[] PropertyMapTable = null;
		public PropertyData[] PropertyTable = null;
		public MethodSemanticsData[] MethodSemanticsTable = null;
		public MethodImplData[] MethodImplTable = null;
		public ModuleRefData[] ModuleRefTable = null;
		public TypeSpecData[] TypeSpecTable = null;
		public ImplMapData[] ImplMapTable = null;
		public FieldRVAData[] FieldRVATable = null;
		public AssemblyData[] AssemblyTable = null;
		public AssemblyProcessorData[] AssemblyProcessorTable = null;
		public AssemblyOSData[] AssemblyOSTable = null;
		public AssemblyRefData[] AssemblyRefTable = null;
		public AssemblyRefProcessorData[] AssemblyRefProcessorTable = null;
		public AssemblyRefOSData[] AssemblyRefOSTable = null;
		public FileData[] FileTable = null;
		public ExportedTypeData[] ExportedTypeTable = null;
		public ManifestResourceData[] ManifestResourceTable = null;
		public NestedClassData[] NestedClassTable = null;
		public GenericParamData[] GenericParamTable = null;
		public MethodSpecData[] MethodSpecTable = null;
		public GenericParamConstraintData[] GenericParamConstraintTable = null;

		public CLIFile(string pReferenceName, byte[] pData)
		{
			mData = pData;

			DOS.Read(this);
			mCursor = (int)DOS.NextHeaderOffset;
			PE.Read(this);
			Optional.Read(this);
			Sections = new SectionHeader[PE.NumberOfSections];
			for (int index = 0; index < Sections.Length; ++index)
			{
				Sections[index] = new SectionHeader();
				Sections[index].Read(this);
			}

			DataDirectory headerDataDirectory = Optional.DataDirectories[14];
			SectionHeader headerSectionHeader = GetSection(headerDataDirectory.VirtualAddress);
			mCursor = (int)(headerSectionHeader.PointerToRawData + (headerDataDirectory.VirtualAddress - headerSectionHeader.VirtualAddress));
			CLI.Read(this);
			SectionHeader metadataSectionHeader = GetSection(CLI.Metadata.VirtualAddress);
			CLIMetadataHeaderOffset = metadataSectionHeader.PointerToRawData + (CLI.Metadata.VirtualAddress - metadataSectionHeader.VirtualAddress);
			mCursor = (int)CLIMetadataHeaderOffset;
			CLIMetadata.Read(this);
			mCursor = (int)CLIMetadataHeaderOffset;
			foreach (CLIMetadataStreamHeader streamHeader in CLIMetadata.Streams)
			{
				switch (streamHeader.Name)
				{
					case "#~": Tables = streamHeader; break;
					case "#Strings": Strings = streamHeader; break;
					case "#US": US = streamHeader; break;
					case "#GUID": GUID = streamHeader; break;
					case "#Blob": Blob = streamHeader; break;
					default: throw new BadImageFormatException("Invalid CLIMetadataHeader Stream");
				}
			}
			mCursor = (int)(CLIMetadataHeaderOffset + Tables.Offset);
			CLIMetadataTables.Read(this);

			Array.ForEach(sMetadataLoaders, l => l.Initializer(this));
			Array.ForEach(sMetadataLoaders, l => l.Loader(this));
			Array.ForEach(sMetadataLoaders, l => l.Linker(this));
			Array.ForEach(MethodDefTable, m => { if (m.Body != null) m.Body.LinkData(m); });
		}

		public byte[] Data { get { return mData; } }

		public SectionHeader GetSection(uint pVirtualAddress)
		{
			for (int index = 0; index < Sections.Length; ++index)
			{
				if (pVirtualAddress >= Sections[index].VirtualAddress &&
					pVirtualAddress < (Sections[index].VirtualAddress + Sections[index].VirtualSize)) return Sections[index];
			}
			throw new BadImageFormatException(string.Format("{0}: No section header for address 0x{1:X}", ReferenceName, pVirtualAddress));
		}

		public MetadataToken ExpandMetadataToken(uint pToken)
		{
			MetadataToken token = new MetadataToken();
			token.Table = (byte)(pToken >> 24);
			token.IsUserString = token.Table == MetadataTables.UserStrings;
			uint index = pToken & 0x00FFFFFF;
			if (index == 0) return token;
			if (token.IsUserString)
			{
				token.Data = ReadUserStringHeap((int)index);
				return token;
			}
			--index;
			switch (token.Table)
			{
				case MetadataTables.Module: token.Data = ModuleTable[index]; break;
				case MetadataTables.TypeRef: token.Data = TypeDefTable[index]; break;
				case MetadataTables.TypeDef: token.Data = TypeDefTable[index]; break;
				case MetadataTables.Field: token.Data = FieldTable[index]; break;
				case MetadataTables.MethodDef: token.Data = MethodDefTable[index]; break;
				case MetadataTables.Param: token.Data = ParamTable[index]; break;
				case MetadataTables.InterfaceImpl: token.Data = InterfaceImplTable[index]; break;
				case MetadataTables.MemberRef: token.Data = MemberRefTable[index]; break;
				case MetadataTables.Constant: token.Data = ConstantTable[index]; break;
				case MetadataTables.CustomAttribute: token.Data = CustomAttributeTable[index]; break;
				case MetadataTables.FieldMarshal: token.Data = FieldMarshalTable[index]; break;
				case MetadataTables.DeclSecurity: token.Data = DeclSecurityTable[index]; break;
				case MetadataTables.ClassLayout: token.Data = ClassLayoutTable[index]; break;
				case MetadataTables.FieldLayout: token.Data = FieldLayoutTable[index]; break;
				case MetadataTables.StandAloneSig: token.Data = StandAloneSigTable[index]; break;
				case MetadataTables.EventMap: token.Data = EventMapTable[index]; break;
				case MetadataTables.Event: token.Data = EventTable[index]; break;
				case MetadataTables.PropertyMap: token.Data = PropertyMapTable[index]; break;
				case MetadataTables.Property: token.Data = PropertyTable[index]; break;
				case MetadataTables.MethodSemantics: token.Data = MethodSemanticsTable[index]; break;
				case MetadataTables.MethodImpl: token.Data = MethodImplTable[index]; break;
				case MetadataTables.ModuleRef: token.Data = ModuleRefTable[index]; break;
				case MetadataTables.TypeSpec: token.Data = TypeSpecTable[index]; break;
				case MetadataTables.ImplMap: token.Data = ImplMapTable[index]; break;
				case MetadataTables.FieldRVA: token.Data = FieldRVATable[index]; break;
				case MetadataTables.Assembly: token.Data = AssemblyTable[index]; break;
				case MetadataTables.AssemblyProcessor: token.Data = AssemblyProcessorTable[index]; break;
				case MetadataTables.AssemblyOS: token.Data = AssemblyOSTable[index]; break;
				case MetadataTables.AssemblyRef: token.Data = AssemblyRefTable[index]; break;
				case MetadataTables.AssemblyRefProcessor: token.Data = AssemblyRefProcessorTable[index]; break;
				case MetadataTables.AssemblyRefOS: token.Data = AssemblyRefOSTable[index]; break;
				case MetadataTables.File: token.Data = FileTable[index]; break;
				case MetadataTables.ExportedType: token.Data = ExportedTypeTable[index]; break;
				case MetadataTables.ManifestResource: token.Data = ManifestResourceTable[index]; break;
				case MetadataTables.NestedClass: token.Data = NestedClassTable[index]; break;
				case MetadataTables.GenericParam: token.Data = GenericParamTable[index]; break;
				case MetadataTables.MethodSpec: token.Data = MethodSpecTable[index]; break;
				case MetadataTables.GenericParamConstraint: token.Data = GenericParamConstraintTable[index]; break;
				default: break;
			}
			return token;
		}

		public MetadataToken ExpandTypeDefRefOrSpecToken(uint pToken)
		{
			MetadataToken token = new MetadataToken();
			token.Table = (byte)(pToken & 0x03);
			switch (token.Table)
			{
				case 0: token.Table = MetadataTables.TypeDef; break;
				case 1: token.Table = MetadataTables.TypeRef; break;
				case 2: token.Table = MetadataTables.TypeSpec; break;
				default: return token;
			}
			uint index = pToken >> 2;
			if (index == 0) return token;
			--index;
			switch (token.Table)
			{
				case MetadataTables.TypeDef: token.Data = TypeDefTable[index]; break;
				case MetadataTables.TypeRef: token.Data = TypeRefTable[index]; break;
				case MetadataTables.TypeSpec: token.Data = TypeSpecTable[index]; break;
				default: break;
			}
			return token;
		}

		#region Read Methods
		public byte ReadByte()
		{
			byte value = mData[mCursor];
			mCursor += 1;
			return value;
		}

		public sbyte ReadSByte()
		{
			sbyte value = (sbyte)mData[mCursor];
			mCursor += 1;
			return value;
		}

		public ushort ReadUInt16()
		{
			ushort value = mData[mCursor];
			value |= (ushort)(mData[mCursor + 1] << 8);
			mCursor += 2;
			return value;
		}

		public short ReadInt16()
		{
			short value = mData[mCursor];
			value |= (short)(mData[mCursor + 1] << 8);
			mCursor += 2;
			return value;
		}

		public uint ReadUInt32()
		{
			uint value = mData[mCursor];
			value |= (uint)(mData[mCursor + 1] << 8);
			value |= (uint)(mData[mCursor + 2] << 16);
			value |= (uint)(mData[mCursor + 3] << 24);
			mCursor += 4;
			return value;
		}

		public int ReadInt32()
		{
			int value = mData[mCursor];
			value |= (int)(mData[mCursor + 1] << 8);
			value |= (int)(mData[mCursor + 2] << 16);
			value |= (int)(mData[mCursor + 3] << 24);
			mCursor += 4;
			return value;
		}

		public ulong ReadUInt64()
		{
			ulong value = mData[mCursor];
			value |= (ulong)((ulong)mData[mCursor + 1] << 8);
			value |= (ulong)((ulong)mData[mCursor + 2] << 16);
			value |= (ulong)((ulong)mData[mCursor + 3] << 24);
			value |= (ulong)((ulong)mData[mCursor + 4] << 32);
			value |= (ulong)((ulong)mData[mCursor + 5] << 40);
			value |= (ulong)((ulong)mData[mCursor + 6] << 48);
			value |= (ulong)((ulong)mData[mCursor + 7] << 56);
			mCursor += 8;
			return value;
		}

		public long ReadInt64()
		{
			long value = mData[mCursor];
			value |= (long)((long)mData[mCursor + 1] << 8);
			value |= (long)((long)mData[mCursor + 2] << 16);
			value |= (long)((long)mData[mCursor + 3] << 24);
			value |= (long)((long)mData[mCursor + 4] << 32);
			value |= (long)((long)mData[mCursor + 5] << 40);
			value |= (long)((long)mData[mCursor + 6] << 48);
			value |= (long)((long)mData[mCursor + 7] << 56);
			mCursor += 8;
			return value;
		}

		public byte[] ReadBytes(int pLength)
		{
			byte[] value = new byte[pLength];
			Buffer.BlockCopy(mData, mCursor, value, 0, pLength);
			mCursor += pLength;
			return value;
		}

		public string ReadString()
		{
			int length = 0;
			while (mData[mCursor + length] != 0x00)
			{
				++length;
				if (length >= mData.Length) return "";
			}
			string value = "";
			if (length > 0) value = Encoding.ASCII.GetString(mData, mCursor, length);
			mCursor += length + 1;
			return value;
		}

		public string ReadString(int pLength)
		{
			string value = Encoding.ASCII.GetString(mData, mCursor, pLength);
			mCursor += pLength;
			return value;
		}

        public int ReadHeapIndex(byte p32BitFlag)
        {
            int heapIndex = 0;
            if ((CLIMetadataTables.HeapOffsetSizes & p32BitFlag) != 0) heapIndex = ReadInt32();
            else heapIndex = ReadInt16();
            return heapIndex;
        }

        public static uint ReadCompressedUnsigned(byte[] pBuffer, ref int pOffset)
        {
            uint value = 0;
            if ((pBuffer[pOffset] & 0x80) == 0)
            {
                value = (uint)(pBuffer[pOffset] & 0x7F);
                pOffset += 1;
                return value;
            }
            if ((pBuffer[pOffset] & 0xC0) == 0x80)
            {
                value = (uint)(((pBuffer[pOffset] & 0x3F) << 8) + pBuffer[pOffset + 1]);
                pOffset += 2;
                return value;
            }
            if ((pBuffer[pOffset] & 0xE0) == 0xC0)
            {
                value = (uint)(((pBuffer[pOffset] & 0x1F) << 24) + (pBuffer[pOffset + 1] << 16) + (pBuffer[pOffset + 2] << 8) + pBuffer[pOffset + 3]);
                pOffset += 4;
                return value;
            }
            value = 0;
            return value;
        }

        public static int ReadCompressedSigned(byte[] pBuffer, ref int pOffset)
        {
            uint value = 0;
            if ((pBuffer[pOffset] & 0x80) == 0)
            {
                value = (uint)(pBuffer[pOffset] & 0x7F);
                value = RotateRight(value, 7);
                if ((value & 0x40) != 0) value |= 0xFFFFFF80;
                pOffset += 1;
                return (int)value;
            }
            if ((pBuffer[pOffset] & 0xC0) == 0x80)
            {
                value = (uint)(((pBuffer[pOffset] & 0x3F) << 8) + pBuffer[pOffset + 1]);
                value = RotateRight(value, 14);
                if ((value & 0x2000) != 0) value |= 0xFFFFC000;
                pOffset += 2;
                return (int)value;
            }
            if ((pBuffer[pOffset] & 0xE0) == 0xC0)
            {
                value = (uint)(((pBuffer[pOffset] & 0x1F) << 24) + (pBuffer[pOffset + 1] << 16) + (pBuffer[pOffset + 2] << 8) + pBuffer[pOffset + 3]);
                value = RotateRight(value, 29);
                if ((value & 0x10000000) != 0) value |= 0xE0000000;
                pOffset += 4;
                return (int)value;
            }
            value = 0;
            return (int)value;
        }

        private static uint RotateRight(uint pValue, byte pBits)
        {
            bool bit = (pValue & 0x01) != 0;
            pValue >>= 1;
            if (bit) pValue |= (uint)(1 << (pBits - 1));
            return pValue;
        }

        public string ReadStringHeap(int pHeapOffset)
        {
            int offset = (int)(CLIMetadataHeaderOffset + Strings.Offset + pHeapOffset);
            int length = 0;
            while (Data[offset + length] != 0x00) ++length;
            return Encoding.ASCII.GetString(Data, offset, length);
        }

        public string ReadUserStringHeap(int pHeapOffset)
        {
            int offset = (int)(CLIMetadataHeaderOffset + US.Offset + pHeapOffset);
            uint length = ReadCompressedUnsigned(Data, ref offset);
            if (length == 0 || length == 1) return "";
            --length;
            return Encoding.Unicode.GetString(Data, offset, (int)length);
        }

        public byte[] ReadBlobHeap(int pHeapOffset)
        {
            int offset = (int)(CLIMetadataHeaderOffset + Blob.Offset + pHeapOffset);
            uint length = ReadCompressedUnsigned(Data, ref offset);
            byte[] blob = new byte[length];
            Buffer.BlockCopy(Data, offset, blob, 0, (int)length);
            return blob;
        }

        public byte[] ReadGUIDHeap(int pHeapOffset)
        {
            int offset = (int)(CLIMetadataHeaderOffset + GUID.Offset + pHeapOffset);
            byte[] guid = new byte[8];
            Buffer.BlockCopy(Data, offset, guid, 0, 8);
            return guid;
        }
		#endregion

		#region Headers
		public sealed class DOSHeader
		{
			public ushort Signature;
			public ushort LastPageByteCount;
			public ushort PageCount;
			public ushort RelocationCount;
			public ushort HeaderParagraphCount;
			public ushort MinimumParagraphCount;
			public ushort MaximumParagraphCount;
			public ushort RegisterSS;
			public ushort RegisterSP;
			public ushort Checksum;
			public ushort RegisterIP;
			public ushort RegisterCS;
			public ushort RelocationOffset;
			public ushort Overlay;
			public ushort[] ReservedA = new ushort[4];
			public ushort OEMIdentifier;
			public ushort OEMInformation;
			public ushort[] ReservedB = new ushort[10];
			public uint NextHeaderOffset;

			public void Read(CLIFile pFile)
			{
				Signature = pFile.ReadUInt16();
				LastPageByteCount = pFile.ReadUInt16();
				PageCount = pFile.ReadUInt16();
				RelocationCount = pFile.ReadUInt16();
				HeaderParagraphCount = pFile.ReadUInt16();
				MinimumParagraphCount = pFile.ReadUInt16();
				MaximumParagraphCount = pFile.ReadUInt16();
				RegisterSS = pFile.ReadUInt16();
				RegisterSP = pFile.ReadUInt16();
				Checksum = pFile.ReadUInt16();
				RegisterIP = pFile.ReadUInt16();
				RegisterCS = pFile.ReadUInt16();
				RelocationOffset = pFile.ReadUInt16();
				Overlay = pFile.ReadUInt16();
				for (int index = 0; index < ReservedA.Length; ++index) ReservedA[index] = pFile.ReadUInt16();
				OEMIdentifier = pFile.ReadUInt16();
				OEMInformation = pFile.ReadUInt16();
				for (int index = 0; index < ReservedB.Length; ++index) ReservedB[index] = pFile.ReadUInt16();
				NextHeaderOffset = pFile.ReadUInt32();
			}
		}

		public sealed class PEHeader
		{
			public uint Signature;
			public ushort Machine;
			public ushort NumberOfSections;
			public uint Timestamp;
			public uint PointerToSymbolTable;
			public uint NumberOfSymbols;
			public ushort SizeOfOptionalHeader;
			public ushort Characteristics;

			public void Read(CLIFile pFile)
			{
				Signature = pFile.ReadUInt32();
				Machine = pFile.ReadUInt16();
				NumberOfSections = pFile.ReadUInt16();
				Timestamp = pFile.ReadUInt32();
				PointerToSymbolTable = pFile.ReadUInt32();
				NumberOfSymbols = pFile.ReadUInt32();
				SizeOfOptionalHeader = pFile.ReadUInt16();
				Characteristics = pFile.ReadUInt16();
			}
		}

		public sealed class DataDirectory
		{
			public uint VirtualAddress;
			public uint Size;

			public void Read(CLIFile pFile)
			{
				VirtualAddress = pFile.ReadUInt32();
				Size = pFile.ReadUInt32();
			}
		}

		public sealed class OptionalHeader
		{
			public ushort Magic;
			public byte MajorLinkerVersion;
			public byte MinorLinkerVersion;
			public uint SizeOfCode;
			public uint SizeOfInitializedData;
			public uint SizeOfUninitializedData;
			public uint AddressOfEntryPoint;
			public uint BaseOfCode;
			public uint BaseOfData;

			public uint ImageBase;
			public uint SectionAlignment;
			public uint FileAlignment;
			public ushort MajorOperatingSystemVersion;
			public ushort MinorOperatingSystemVersion;
			public ushort MajorImageVersion;
			public ushort MinorImageVersion;
			public ushort MajorSubsystemVersion;
			public ushort MinorSubsystemVersion;
			public uint Reserved;
			public uint SizeOfImage;
			public uint SizeOfHeaders;
			public uint Checksum;
			public ushort Subsystem;
			public ushort DllCharacteristics;
			public uint SizeOfStackReserve;
			public uint SizeOfStackCommit;
			public uint SizeOfHeapReserve;
			public uint SizeOfHeapCommit;
			public uint LoaderFlags;
			public uint NumberOfRVAAndSizes;
			public DataDirectory[] DataDirectories = new DataDirectory[16];

			public bool Read(CLIFile pFile)
			{
				Magic = pFile.ReadUInt16();
				MajorLinkerVersion = pFile.ReadByte();
				MinorLinkerVersion = pFile.ReadByte();
				SizeOfCode = pFile.ReadUInt32();
				SizeOfInitializedData = pFile.ReadUInt32();
				SizeOfUninitializedData = pFile.ReadUInt32();
				AddressOfEntryPoint = pFile.ReadUInt32();
				BaseOfCode = pFile.ReadUInt32();
				BaseOfData = pFile.ReadUInt32();
				ImageBase = pFile.ReadUInt32();
				SectionAlignment = pFile.ReadUInt32();
				FileAlignment = pFile.ReadUInt32();
				MajorOperatingSystemVersion = pFile.ReadUInt16();
				MinorOperatingSystemVersion = pFile.ReadUInt16();
				MajorImageVersion = pFile.ReadUInt16();
				MinorImageVersion = pFile.ReadUInt16();
				MajorSubsystemVersion = pFile.ReadUInt16();
				MinorSubsystemVersion = pFile.ReadUInt16();
				Reserved = pFile.ReadUInt32();
				SizeOfImage = pFile.ReadUInt32();
				SizeOfHeaders = pFile.ReadUInt32();
				Checksum = pFile.ReadUInt32();
				Subsystem = pFile.ReadUInt16();
				DllCharacteristics = pFile.ReadUInt16();
				SizeOfStackReserve = pFile.ReadUInt32();
				SizeOfStackCommit = pFile.ReadUInt32();
				SizeOfHeapReserve = pFile.ReadUInt32();
				SizeOfHeapCommit = pFile.ReadUInt32();
				LoaderFlags = pFile.ReadUInt32();
				NumberOfRVAAndSizes = pFile.ReadUInt32();
				for (int index = 0; index < DataDirectories.Length; ++index)
				{
					DataDirectories[index] = new DataDirectory();
					DataDirectories[index].Read(pFile);
				}
				return true;
			}
		}

		public sealed class SectionHeader
		{
			public string Name;
			public uint PhysicalAddress;
			public uint VirtualSize;
			public uint VirtualAddress;
			public uint SizeOfRawData;
			public uint PointerToRawData;
			public uint PointerToRelocations;
			public uint PointerToLineNumbers;
			public ushort NumberOfRelocations;
			public ushort NumberOfLineNumbers;
			public uint Characteristics;

			public void Read(CLIFile pFile)
			{
				Name = pFile.ReadString(8);
				PhysicalAddress = VirtualSize = pFile.ReadUInt32();
				VirtualAddress = pFile.ReadUInt32();
				SizeOfRawData = pFile.ReadUInt32();
				PointerToRawData = pFile.ReadUInt32();
				PointerToRelocations = pFile.ReadUInt32();
				PointerToLineNumbers = pFile.ReadUInt32();
				NumberOfRelocations = pFile.ReadUInt16();
				NumberOfLineNumbers = pFile.ReadUInt16();
				Characteristics = pFile.ReadUInt32();
			}
		}

		public sealed class CLIHeader
		{
			public uint SizeOfHeader;
			public ushort MajorRuntimeVersion;
			public ushort MinorRuntimeVersion;
			public DataDirectory Metadata = new DataDirectory();
			public uint Flags;
			public uint EntryPointToken;
			public ushort EntryPointRVA;
			public DataDirectory Resources = new DataDirectory();
			public DataDirectory StrongNameSignature = new DataDirectory();
			public DataDirectory CodeManagerTable = new DataDirectory();
			public DataDirectory VTableFixups = new DataDirectory();
			public DataDirectory ExportAddressTableJumps = new DataDirectory();
			public DataDirectory ManagedNativeHeader = new DataDirectory();

			public void Read(CLIFile pFile)
			{
				SizeOfHeader = pFile.ReadUInt32();
				MajorRuntimeVersion = pFile.ReadUInt16();
				MinorRuntimeVersion = pFile.ReadUInt16();
				Metadata.Read(pFile);
				Flags = pFile.ReadUInt32();
				EntryPointToken = pFile.ReadUInt32();
				EntryPointRVA = pFile.ReadUInt16();
				Resources.Read(pFile);
				StrongNameSignature.Read(pFile);
				CodeManagerTable.Read(pFile);
				VTableFixups.Read(pFile);
				ExportAddressTableJumps.Read(pFile);
				ManagedNativeHeader.Read(pFile);
			}
		}

		public sealed class CLIMetadataStreamHeader
		{
			public uint Offset;
			public uint Size;
			public string Name;

			public void Read(CLIFile pFile)
			{
				Offset = pFile.ReadUInt32();
				Size = pFile.ReadUInt32();
				Name = pFile.ReadString();
				uint headerSize = (uint)(sizeof(uint) * 2 + (Name.Length + 1));
				if ((headerSize & 0x03) != 0) pFile.mCursor += (int)(4 - (headerSize & 0x03));
			}
		}

		public sealed class CLIMetadataHeader
		{
			public uint Signature;
			public ushort MajorVersion;
			public ushort MinorVersion;
			public uint Reserved;
			public uint VersionLength;
			public byte[] Version;
			public ushort Flags;
			public ushort StreamCount;
			public CLIMetadataStreamHeader[] Streams;

			public bool Read(CLIFile pFile)
			{
				Signature = pFile.ReadUInt32();
				MajorVersion = pFile.ReadUInt16();
				MinorVersion = pFile.ReadUInt16();
				Reserved = pFile.ReadUInt32();
				VersionLength = pFile.ReadUInt32();
				Version = pFile.ReadBytes((int)VersionLength);
				Flags = pFile.ReadUInt16();
				StreamCount = pFile.ReadUInt16();

				Streams = new CLIMetadataStreamHeader[StreamCount];
				for (int index = 0; index < Streams.Length; ++index)
				{
					Streams[index] = new CLIMetadataStreamHeader();
					Streams[index].Read(pFile);
				}
				return true;
			}
		}

		public sealed class CLIMetadataTablesHeader
		{
			public uint ReservedA;
			public byte MajorVersion;
			public byte MinorVersion;
			public byte HeapOffsetSizes;
			public byte ReservedB;
			public ulong PresentTables;
			public ulong SortedTables;

			public void Read(CLIFile pFile)
			{
				ReservedA = pFile.ReadUInt32();
				MajorVersion = pFile.ReadByte();
				MinorVersion = pFile.ReadByte();
				HeapOffsetSizes = pFile.ReadByte();
				ReservedB = pFile.ReadByte();
				PresentTables = pFile.ReadUInt64();
				SortedTables = pFile.ReadUInt64();
			}
		}
		#endregion
	}
}
