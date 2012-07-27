#include <CLR/JIT.h>
#include <CLR/JIT/x86/Layout.h>

const uint32_t gSizeOfPointerInBytes = 4;
const uint32_t gPointerDivideShift = 2;

uint32_t JIT_GetStackSizeOfType(IRType* pType)
{
	if (pType->StackSizeCalculated) return pType->StackSize;

	if (pType->IsValueType)
	{
		AppDomain* domain = pType->ParentAssembly->ParentDomain;
		if (pType->TypeDefinition->ClassLayout)
		{
			pType->StackSize = pType->TypeDefinition->ClassLayout->ClassSize;
		}
		else if (pType->TypeDefinition == domain->CachedType___System_Byte ||
			     pType->TypeDefinition == domain->CachedType___System_SByte ||
			     pType->TypeDefinition == domain->CachedType___System_Boolean)
		{
			pType->StackSize = 1;
		}
		else if (pType->TypeDefinition == domain->CachedType___System_Int16 ||
			     pType->TypeDefinition == domain->CachedType___System_UInt16 ||
			     pType->TypeDefinition == domain->CachedType___System_Char)
		{
			pType->StackSize = 2;
		}
		else if (pType->TypeDefinition == domain->CachedType___System_Int32 ||
			     pType->TypeDefinition == domain->CachedType___System_UInt32 ||
			     pType->TypeDefinition == domain->CachedType___System_Single)
		{
			pType->StackSize = 4;
		}
		else if (pType->TypeDefinition == domain->CachedType___System_Int64 ||
			     pType->TypeDefinition == domain->CachedType___System_UInt64 ||
			     pType->TypeDefinition == domain->CachedType___System_Double)
		{
			pType->StackSize = 8;
		}
		else if (pType->TypeDefinition == domain->CachedType___System_IntPtr ||
			     pType->TypeDefinition == domain->CachedType___System_UIntPtr)
		{
			pType->StackSize = gSizeOfPointerInBytes;
		}
		else
		{
			uint32_t totalSize = 0;
			for (uint32_t index = 0; index < pType->FieldCount; ++index)
			{
				if (pType->Fields[index]->FieldType->IsReferenceType) totalSize += gSizeOfPointerInBytes;
				else totalSize += JIT_GetStackSizeOfType(pType->Fields[index]->FieldType);
			}
			pType->StackSize = totalSize;
		}
	}
	else pType->StackSize = gSizeOfPointerInBytes;
	pType->StackSizeCalculated = TRUE;
	return pType->StackSize;
}

uint32_t JIT_GetSizeOfType(IRType* pType)
{
	if (pType->SizeCalculated) return pType->Size;
	if (pType->IsValueType) pType->Size = JIT_GetStackSizeOfType(pType);
	else
	{
		uint32_t sizeOfType = 0;
		for (uint32_t index = 0; index < pType->FieldCount; ++index)
		{
			sizeOfType += JIT_GetStackSizeOfType(pType->Fields[index]->FieldType);
		}
		pType->Size = sizeOfType;
	}
	pType->SizeCalculated = TRUE;
	return pType->Size;
}

void JIT_CalculateParameterLayout(IRMethod* pMethod)
{
	if (!pMethod->ParametersLayedOut)
	{
		IRParameter* parameter = NULL;
		// Accounts for caller IRMethod*, AppDomain*, Reent*, return pointer, and saved stack frame in parameters space
		uint32_t offset = 2 * gSizeOfPointerInBytes;
		Log_WriteLine(LOGLEVEL__JIT_Layout, "Laying Out Parameters of %s.%s.%s", pMethod->MethodDefinition->TypeDefinition->Namespace, pMethod->MethodDefinition->TypeDefinition->Name, pMethod->MethodDefinition->Name);
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
		uint32_t offset = 0;
		Log_WriteLine(LOGLEVEL__JIT_Layout, "Laying Out Locals of %s.%s.%s", pMethod->MethodDefinition->TypeDefinition->Namespace, pMethod->MethodDefinition->TypeDefinition->Name, pMethod->MethodDefinition->Name);
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

void JIT_CalculateFieldLayout(IRType* pType)
{
	if (!pType->FieldsLayedOut)
	{
		IRField* field = NULL;
		uint32_t offset = 0;
		Log_WriteLine(LOGLEVEL__JIT_Layout, "Laying Out Fields of %s.%s", pType->TypeDefinition->Namespace, pType->TypeDefinition->Name);
		for (uint32_t index = 0; index < pType->FieldCount; ++index)
		{
			field = pType->Fields[index];
			field->Size = JIT_GetStackSizeOfType(field->FieldType);
			field->Offset = offset;
			offset += field->Size;
			Log_WriteLine(LOGLEVEL__JIT_Layout, "Layout Field %u @ 0x%x, Size: 0x%x", (unsigned int)index, (unsigned int)field->Offset, (unsigned int)field->Size);
		}
		pType->FieldsLayedOut = TRUE;
	}
}

void JIT_CalculateStaticFieldLayout(IRAssembly* pAssembly)
{
	IRField* field = NULL;
	uint32_t offset = 0;
	uint32_t totalSize = 0;
	Log_WriteLine(LOGLEVEL__JIT_Layout, "Laying Out Static Fields of %s", pAssembly->ParentFile->Filename);
	for (uint32_t index = 0; index < pAssembly->StaticFieldCount; ++index)
	{
		field = pAssembly->StaticFields[index];
		field->Size = JIT_GetStackSizeOfType(field->FieldType);
		field->Offset = offset;
		offset += field->Size;
		totalSize += field->Size;
		Log_WriteLine(LOGLEVEL__JIT_Layout, "Layout Static Field %u @ 0x%x, Size: 0x%x", (unsigned int)index, (unsigned int)field->Offset, (unsigned int)field->Size);
	}
	pAssembly->ParentDomain->StaticValues[pAssembly->AssemblyIndex] = realloc(pAssembly->ParentDomain->StaticValues[pAssembly->AssemblyIndex], totalSize);
}