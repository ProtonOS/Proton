#pragma once

typedef struct _IRAssembly IRAssembly;
typedef struct _IRGenericParameter IRGenericParameter;
typedef struct _IRType IRType;
typedef struct _IRMethod IRMethod;
typedef struct _IRParameter IRParameter;
typedef struct _IRLocalVariable IRLocalVariable;
typedef struct _IRField IRField;
typedef struct _IRArrayType IRArrayType;
typedef struct _IRInterfaceImpl IRInterfaceImpl;
typedef struct _IRInstruction IRInstruction;
typedef struct _IRPointerType IRPointerType;
typedef struct _IRGenericType IRGenericType;
typedef struct _IRGenericMethod IRGenericMethod;

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
	
	uint32_t StaticFieldIndex;
	uint32_t StaticFieldCount;
	IRField** StaticFields;

	IRArrayType* ArrayTypesHashTable;
	IRPointerType* PointerTypesHashTable;
	IRGenericType* GenericTypesHashTable;
	IRGenericMethod* GenericMethodsHashTable;
};

IRAssembly* IRAssembly_Create(AppDomain* pDomain, CLIFile* pFile);
void IRAssembly_Destroy(IRAssembly* pAssembly);
void IRAssembly_AddField(IRAssembly* pAssembly, IRField* pField);
void IRAssembly_AddType(IRAssembly* pAssembly, IRType* pType);
IRType* IRAssembly_MakePointerType(IRAssembly* pAssembly, IRType* pType);
IRType* IRAssembly_MakeArrayType(IRAssembly* pAssembly, IRType* pType);
IRType* IRAssembly_CreateGenericParameter(IRAssembly* pAssembly, bool_t pIsFromParentType, uint32_t pIndex);

struct _IRGenericParameter
{
	bool_t FromParentType;
	uint32_t Index;
};

struct _IRType
{
	uint32_t TypeIndex;
	IRAssembly* ParentAssembly;
	TypeDefinition* TypeDefinition;

    bool_t IsValueType :1;
    bool_t IsReferenceType :1;
	bool_t IsArrayType :1;
	bool_t IsPointerType :1;
	bool_t IsGeneric :1;
	bool_t IsGenericInstantiation :1;
	bool_t IsInterface :1;
	bool_t IsAbstract :1;
	// Above is 1 byte.
    
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

	IRGenericType* GenericType;

	uint32_t GenericParameterCount;

	bool_t IsGenericParameter;
	bool_t IsGenericParameterFromParentType;
	uint32_t GenericParameterIndex;
};

IRType* IRType_Create(IRAssembly* pAssembly, TypeDefinition* pTypeDefinition);
IRType* IRType_Copy(IRType* pType);
IRType* IRType_GenericDeepCopy(IRType* pType, IRAssembly* pAssembly);
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

	bool_t IsGeneric;
	bool_t IsGenericImplementation;
	uint32_t GenericParameterCount;
	IRGenericMethod* GenericMethod;

	bool_t IsDomainSpecific;

    void(*AssembledMethod)();
};

IRMethod* IRMethod_Create(IRAssembly* pAssembly, MethodDefinition* pMethodDefinition);
IRMethod* IRMethod_GenericDeepCopy(IRMethod* pMethod, IRAssembly* pAssembly);
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
IRParameter* IRParameter_Copy(IRParameter* pParameter);
void IRParameter_Destroy(IRParameter* pParameter);

struct _IRLocalVariable
{
	uint32_t LocalVariableIndex;
    IRType* VariableType;
	uint32_t Offset;
	uint32_t Size;
	bool_t Pinned;

	IRLocalVariable* Ancestor;
	bool_t Used;
};

IRLocalVariable* IRLocalVariable_Create(IRMethod* pMethod, IRType* pType);
IRLocalVariable* IRLocalVariable_Copy(IRLocalVariable* pLocalVariable);
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
	uint32_t StaticFieldIndex;
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

#define IRGENERICTYPE__Max_Parameters			32

struct _IRGenericType
{
	IRType* GenericType;

	uint32_t ParameterCount;
	IRType* Parameters[IRGENERICTYPE__Max_Parameters];

	// This must come after the other parts, for keying reasons
	IRType* ImplementationType;

	UT_hash_handle HashHandle;
};

IRGenericType* IRGenericType_Create(IRType* pGenericType, IRType* pImplementationType);
void IRGenericType_Destroy(IRGenericType* pGenericType);

#define IRGENERICMETHOD__Max_Parameters			32

struct _IRGenericMethod
{
	IRType* ParentType;
	IRMethod* GenericMethod;

	uint32_t ParameterCount;
	IRType* Parameters[IRGENERICMETHOD__Max_Parameters];

	// This must come after the other parts, for keying reasons
	IRMethod* ImplementationMethod;

	UT_hash_handle HashHandle;
};

IRGenericMethod* IRGenericMethod_Create(IRType* pParentType, IRMethod* pGenericMethod, IRMethod* pImplementationMethod);
void IRGenericMethod_Destroy(IRGenericMethod* pGenericType);

typedef enum SourceType
{
	SourceType_Local,
	SourceType_Parameter,
	SourceType_ConstantI4,
	SourceType_ConstantI8,
	SourceType_ConstantR4,
	SourceType_ConstantR8,
	SourceType_Field,
	SourceType_StaticField,
} SourceType;

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

	SourceType Source1Type;

};

IRInstruction* IRInstruction_Create(uint32_t pILLocation, IROpcode pOpcode);
IRInstruction* IRInstruction_Copy(IRInstruction* pInstruction);
void IRInstruction_Destroy(IRInstruction* pInstruction);
