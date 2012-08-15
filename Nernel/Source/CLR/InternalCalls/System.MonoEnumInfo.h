#pragma once

#include <CLR/AppDomain.h>

typedef struct _MonoEnumInfo
{
	void* Type;
	void* Values;
	void** Names;
	void* NameHash;
} MonoEnumInfo;

void System_MonoEnumInfo_get_enum_info(AppDomain* pAppDomain, void* pType, MonoEnumInfo* pInfo);
