#include <stdio.h>

#include <CLR/MetaData.h>

bool_t MetaData_IsValidSignature(const uint8_t* pMetaDataHeader) 
{
	return *(uint32_t*)(pMetaDataHeader + MetaData_Offset_Signature) == MetaData_Signature;
}

uint32_t MetaData_GetVersionLength(const uint8_t* pMetaDataHeader)
{
	return *(uint32_t*)(pMetaDataHeader + MetaData_Offset_VersionLength); 
}

const char* MetaData_GetVersion(const uint8_t* pMetaDataHeader) 
{
	return (const char*)(pMetaDataHeader + MetaData_Offset_Version); 
}

uint16_t MetaData_GetStreamCount(const uint8_t* pMetaDataHeader) 
{
	return *(uint16_t*)(pMetaDataHeader + MetaData_Offset_Version + MetaData_GetVersionLength(pMetaDataHeader) + MetaData_Offset_StreamCount);
}

const uint8_t* MetaData_GetStream(const uint8_t* pMetaDataHeader, uint16_t pStreamIndex)
{
    if (pStreamIndex >= MetaData_GetStreamCount(pMetaDataHeader)) return NULL;
    const uint8_t* stream = pMetaDataHeader + MetaData_Offset_Version + MetaData_GetVersionLength(pMetaDataHeader) + MetaData_Offset_Streams;
    while (pStreamIndex > 0)
    {
        stream += MetaData_Stream_Offset_Name;
        while (*stream) ++stream;
        while (!(*stream)) ++stream;
        --pStreamIndex;
    }
    return stream;
}

uint32_t MetaData_GetStreamRVA(const uint8_t* pMetaDataHeader, uint16_t pStreamIndex)
{
    const uint8_t* data = MetaData_GetStream(pMetaDataHeader, pStreamIndex) + MetaData_Stream_Offset_RVA;
    return *(uint32_t*)data;
}

uint32_t MetaData_GetStreamSize(const uint8_t* pMetaDataHeader, uint16_t pStreamIndex)
{
    const uint8_t* data = MetaData_GetStream(pMetaDataHeader, pStreamIndex) + MetaData_Stream_Offset_Size;
    return *(uint32_t*)data;
}

const char* MetaData_GetStreamName(const uint8_t* pMetaDataHeader, uint16_t pStreamIndex)
{
    const uint8_t* data = MetaData_GetStream(pMetaDataHeader, pStreamIndex) + MetaData_Stream_Offset_Name;
    return (const char*)data;
}

const uint8_t* MetaData_GetStreamData(const uint8_t* pMetaDataHeader, uint16_t pStreamIndex)
{
    uint32_t rva = MetaData_GetStreamRVA(pMetaDataHeader, pStreamIndex);
    const uint8_t* data = pMetaDataHeader + rva;
    return data;
}


