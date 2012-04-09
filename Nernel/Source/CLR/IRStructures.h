#pragma once

typedef struct _IRAssembly IRAssembly;
typedef struct _IRType IRType;
typedef struct _IRMethod IRMethod;
typedef struct _IRParameter IRParameter;
typedef struct _IRLocalVariable IRLocalVariable;
typedef struct _IRField IRField;
typedef struct _IRArrayType IRArrayType;
typedef struct _IRInterfaceImpl IRInterfaceImpl;
typedef struct _IRInstruction IRInstruction;
typedef struct _IRPointerType IRPointerType;

#include <uthash.h>
#include <CLR/AppDomain.h>
#include <CLR/CLIFile.h>
#include <CLR/IROpcode.h>

struct _IRAssembly
{
	uint32_t AssemblyIndex;
	AppDomain* ParentDomain;
	CLIFile* ParentFile;

	IRMethod* EntryPoint;

    uint32_t MethodCount;
    IRMethod** Methods;
    uint32_t FieldCount;
    IRField** Fields;
    uint32_t TypeCount;
    IRType** Types;
	
	IRArrayType* ArrayTypesHashTable;
	IRPointerType* PointerTypesHashTable;
};

IRAssembly* IRAssembly_Create(AppDomain* pDomain, CLIFile* pFile);
void IRAssembly_Destroy(IRAssembly* pAssembly);
void IRAssembly_AddField(IRAssembly* pAssembly, IRField* pField);
void IRAssembly_AddType(IRAssembly* pAssembly, IRType* pType);
IRType* IRAssembly_MakePointerType(IRAssembly* pAssembly, IRType* pType);

struct _IRType
{
	uint32_t TypeIndex;
	IRAssembly* ParentAssembly;
	TypeDefinition* TypeDefinition;

    bool_t IsValueType;
    bool_t IsReferenceType;
	bool_t IsArrayType;
	bool_t IsPointerType;
	bool_t IsGeneric;
	bool_t IsGenericInstantiation;
	bool_t IsInterface;
    
	bool_t HasStaticConstructor;
	bool_t StaticConstructorCalled;
	IRMethod* StaticConstructor;

	uint32_t MethodCount;
	IRMethod** Methods;

    uint32_t FieldCount;
    IRField** Fields;
	bool_t FieldsLayedOut;

	bool_t SizeCalculated;
	uint32_t Size;

	bool_t HasFinalizer;
	IRMethod* Finalizer;
	
	IRArrayType* ArrayType;
	IRPointerType* PointerType;

	IRInterfaceImpl* InterfaceTable;
};

IRType* IRType_Create(IRAssembly* pAssembly, TypeDefinition* pTypeDefinition);
IRType* IRType_Copy(IRType* pType);
void IRType_Destroy(IRType* pType);

struct _IRMethod
{
	uint32_t MethodIndex;
	IRAssembly* ParentAssembly;
	MethodDefinition* MethodDefinition;

	bool_t Returns;
	IRType* ReturnType;

	uint32_t ParameterCount;
	IRParameter** Parameters;
	bool_t ParametersLayedOut;

	uint32_t LocalVariableCount;
	IRLocalVariable** LocalVariables;
	bool_t LocalsLayedOut;

	uint32_t IRCodesCount;
	IRInstruction** IRCodes;

    void(*AssembledMethod)();
};

IRMethod* IRMethod_Create(IRAssembly* pAssembly, MethodDefinition* pMethodDefinition);
void IRMethod_Destroy(IRMethod* pMethod);
void IRMethod_AddInstruction(IRMethod* pMethod, IRInstruction* pInstruction);

struct _IRParameter
{
	uint32_t ParameterIndex;
    IRType* Type;
	uint32_t Offset;
	uint32_t Size;
};

IRParameter* IRParameter_Create(IRMethod* pMethod, IRType* pType);
void IRParameter_Destroy(IRParameter* pParameter);

struct _IRLocalVariable
{
	uint32_t LocalVariableIndex;
    IRType* VariableType;
	uint32_t Offset;
	uint32_t Size;
};

IRLocalVariable* IRLocalVariable_Create(IRMethod* pMethod, IRType* pType);
void IRLocalVariable_Destroy(IRLocalVariable* pLocalVariable);

struct _IRField
{
	uint32_t FieldIndex;
	IRAssembly* ParentAssembly;
	Field* FieldDefinition;

	IRType* ParentType;
    IRType* FieldType;

	uint32_t Offset;
	uint32_t Size;

	bool_t IsStatic;
	void* StaticValue;
};

IRField* IRField_Create(IRType* pType, Field* pField);
IRField* IRField_Copy(IRField* pField);
void IRField_Destroy(IRField* pField);

struct _IRArrayType
{
	IRType* ArrayType;
	IRType* ElementType;
	UT_hash_handle HashHandle;
};

IRArrayType* IRArrayType_Create(IRType* pArrayType, IRType* pElementType);
void IRArrayType_Destroy(IRArrayType* pArrayType);

struct _IRPointerType
{
	IRType* PointerType;
	IRType* TypePointedTo;
	UT_hash_handle HashHandle;
};

IRPointerType* IRPointerType_Create(IRType* pPointerType, IRType* pTypePointedTo);
void IRPointerType_Destroy(IRPointerType* pPointerType);

struct _IRInterfaceImpl
{
	IRType* InterfaceType;

	uint32_t MethodCount;
	uint32_t* MethodIndexes;

	UT_hash_handle HashHandle;
};

IRInterfaceImpl* IRInterfaceImpl_Create(IRType* pInterfaceType);
void IRInterfaceImpl_Destroy(IRInterfaceImpl* pInterfaceImpl);

struct _IRInstruction
{
    uint32_t ILLocation;
    uint32_t IRLocation;
    IROpcode Opcode;
    bool_t Arg1NeedsDisposing;
    void* Arg1;
    bool_t Arg2NeedsDisposing;
    void* Arg2;
    bool_t Arg3NeedsDisposing;
    void* Arg3;
    bool_t Arg4NeedsDisposing;
    void* Arg4;
};

IRInstruction* IRInstruction_Create(uint32_t pILLocation, IROpcode pOpcode);
void IRInstruction_Destroy(IRInstruction* pInstruction);