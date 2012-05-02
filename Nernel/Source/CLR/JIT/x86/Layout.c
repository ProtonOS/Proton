#include <CLR/JIT.h>
#include <CLR/JIT/x86/Layout.h>

const uint32_t gSizeOfPointerInBytes = 4;
const uint32_t gPointerDivideShift = 2;

uint32_t JIT_GetStackSizeOfType(IRType* pType)
{
	if (pType->StackSizeCalculated) return pType->StackSize;
	pType->StackSizeCalculated = TRUE;

	if (pType->IsValueType)
	{
		if (pType->TypeDefinition->ClassLayout)
		{
			pType->StackSize = pType->TypeDefinition->ClassLayout->ClassSize;
			return pType->StackSize;
		}
		AppDomain* domain = pType->ParentAssembly->ParentDomain;
		if (pType->TypeDefinition == domain->CachedType___System_Byte ||
			pType->TypeDefinition == domain->CachedType___System_SByte ||
			pType->TypeDefinition == domain->CachedType___System_Boolean)
		{
			pType->StackSize = 1;
			return 1;
		}
		if (pType->TypeDefinition == domain->CachedType___System_Int16 ||
			pType->TypeDefinition == domain->CachedType___System_UInt16 ||
			pType->TypeDefinition == domain->CachedType___System_Char)
		{
			pType->StackSize = 2;
			return 2;
		}
		if (pType->TypeDefinition == domain->CachedType___System_Int32 ||
			pType->TypeDefinition == domain->CachedType___System_UInt32 ||
			pType->TypeDefinition == domain->CachedType___System_Single)
		{
			pType->StackSize = 4;
			return 4;
		}
		if (pType->TypeDefinition == domain->CachedType___System_Int64 ||
			pType->TypeDefinition == domain->CachedType___System_UInt64 ||
			pType->TypeDefinition == domain->CachedType___System_Double)
		{
			pType->StackSize = 8;
			return 8;
		}
		if (pType->TypeDefinition == domain->CachedType___System_IntPtr ||
			pType->TypeDefinition == domain->CachedType___System_UIntPtr)
		{
			pType->StackSize = gSizeOfPointerInBytes;
			return gSizeOfPointerInBytes;
		}

		uint32_t totalSize = 0;
		for (uint32_t index = 0; index < pType->FieldCount; ++index)
		{
			if (pType->Fields[index]->FieldType->IsReferenceType) totalSize += gSizeOfPointerInBytes;
			else totalSize += JIT_GetStackSizeOfType(pType->Fields[index]->FieldType);
		}
		pType->StackSize = totalSize;
		return totalSize;
	}
	pType->StackSize = gSizeOfPointerInBytes;
	return gSizeOfPointerInBytes;
}

uint32_t JIT_GetSizeOfType(IRType* pType)
{
	if (pType->SizeCalculated) return pType->Size;
	pType->SizeCalculated = TRUE;
	if (pType->IsValueType)
	{
		pType->Size = JIT_GetStackSizeOfType(pType);
		return pType->Size;
	}
	uint32_t sizeOfType = 0;
	for (uint32_t index = 0; index < pType->FieldCount; ++index)
	{
		if (pType->Fields[index]->FieldType->IsReferenceType) sizeOfType += gSizeOfPointerInBytes;
		else sizeOfType += JIT_GetStackSizeOfType(pType->Fields[index]->FieldType);
	}
	pType->Size = sizeOfType;
	return sizeOfType;
}

void JIT_CalculateParameterLayout(IRMethod* pMethod)
{
	if (!pMethod->ParametersLayedOut)
	{
		IRParameter* parameter = NULL;
		// Accounts for caller IRMethod*, AppDomain*, Reent*, return pointer, and saved stack frame in parameters space
		uint32_t offset = 5 * gSizeOfPointerInBytes;
		for (uint32_t index = 0; index < pMethod->ParameterCount; ++index)
		{
			parameter = pMethod->Parameters[index];
			parameter->Offset = offset;
			parameter->Size = JIT_GetStackSizeOfType(parameter->Type);
			offset += JIT_StackAlign(parameter->Size);
			Log_WriteLine(LOGLEVEL__JIT_Layout, "Layout Parameter %u @ 0x%x, Size: 0x%x, Aligned: 0x%x", (unsigned int)index, (unsigned int)parameter->Offset, (unsigned int)parameter->Size, (unsigned int)JIT_StackAlign(parameter->Size));
		}
		pMethod->ParametersLayedOut = TRUE;
	}
}

void JIT_CalculateLocalLayout(IRMethod* pMethod)
{
	if (!pMethod->LocalsLayedOut)
	{
		IRLocalVariable* local = NULL;
		// Accounts for current IRMethod* in locals space
		uint32_t offset = gSizeOfPointerInBytes;
		for (uint32_t index = 0; index < pMethod->LocalVariableCount; ++index)
		{
			local = pMethod->LocalVariables[index];
			local->Size = JIT_GetStackSizeOfType(local->VariableType);
			offset += JIT_StackAlign(local->Size);
			local->Offset = offset;
			Log_WriteLine(LOGLEVEL__JIT_Layout, "Layout Local %u @ 0x%x, Size: 0x%x, Aligned: 0x%x", (unsigned int)index, (unsigned int)local->Offset, (unsigned int)local->Size, (unsigned int)JIT_StackAlign(local->Size));
		}
		pMethod->LocalsLayedOut = TRUE;
	}
}
