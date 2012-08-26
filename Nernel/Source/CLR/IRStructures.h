#pragma once

typedef struct _IRAssembly IRAssembly;
typedef struct _IRGenericParameter IRGenericParameter;
typedef struct _IRType IRType;
typedef struct _IRMethod IRMethod;
typedef struct _IRParameter IRParameter;
typedef struct _IRLocalVariable IRLocalVariable;
typedef struct _IRLocalSSAData IRLocalSSAData;
typedef struct _IRField IRField;
typedef struct _IRArrayType IRArrayType;
typedef struct _IRInterfaceImpl IRInterfaceImpl;
typedef struct _IRInstruction IRInstruction;
typedef struct _IRPointerType IRPointerType;
typedef struct _IRGenericType IRGenericType;
typedef struct _IRGenericMethod IRGenericMethod;
typedef struct _TypedReference TypedReference;

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
	bool_t IsStructureType :1;
	bool_t IsArrayType :1;
	bool_t IsPointerType :1;
	bool_t IsGeneric :1;
	bool_t IsGenericInstantiation :1;
	bool_t IsInterface :1;
	// Above is 1 byte.
	bool_t IsAbstract;
    
	bool_t HasStaticConstructor;
	IRMethod* StaticConstructor;

	uint32_t MethodCount;
	IRMethod** Methods;

    uint32_t FieldCount;
    IRField** Fields;
	bool_t FieldsLayedOut;

	//uint32_t NestedTypeCount;
	//IRType** NestedTypes;

	bool_t StackSizeCalculated;
	uint32_t StackSize;
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
void IRType_GenericDeepCopy_Finalize(IRType* pType, IRAssembly* pAssembly, IRType* type);
IRType* IRType_GenericDeepCopy(IRType* pType, IRAssembly* pAssembly);
void IRType_Destroy(IRType* pType);
bool_t IRType_IsSubclassOf(IRType* pType, IRType* pParentType);
bool_t IRType_ContainsReferenceType(IRType* pType);

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
	uint32_t LocalsSize;

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
void IRMethod_InsertInstruction(IRMethod* pMethod, uint32_t pInsertIndex, IRInstruction* pInstruction);
void IRMethod_AddLocal(IRMethod* pMethod, IRLocalVariable* pLocal);

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
	bool_t NonSSADerivable;

	IRLocalSSAData* SSAData;
};

IRLocalVariable* IRLocalVariable_Create(IRMethod* pMethod, IRType* pType);
IRLocalVariable* IRLocalVariable_Copy(IRLocalVariable* pLocalVariable);
void IRLocalVariable_Destroy(IRLocalVariable* pLocalVariable);

struct _IRLocalSSAData
{
	bool_t FirstAssignmentDone;
	IRLocalVariable* Derived;
	uint32_t Iteration;
	IRInstruction* LifeStarted;
	IRInstruction* LifeEnded;
};

IRLocalSSAData* IRLocalSSAData_Create(IRLocalVariable* pDerivedLocalVariable);
IRLocalSSAData* IRLocalSSAData_Copy(IRLocalSSAData* pLocalSSAData);
void IRLocalSSAData_Destroy(IRLocalSSAData* pLocalSSAData);

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
void IRGenericMethod_Destroy(IRGenericMethod* pGenericMethod);

typedef struct _SourceTypeData SourceTypeData;

typedef enum SourceType
{
	SourceType_Null,
	SourceType_Local,
	SourceType_LocalAddress,
	SourceType_Parameter,
	SourceType_ParameterAddress,
	SourceType_ConstantI4,
	SourceType_ConstantI8,
	SourceType_ConstantR4,
	SourceType_ConstantR8,
	SourceType_Field,
	SourceType_FieldAddress,
	SourceType_StaticField,
	SourceType_StaticFieldAddress,
	SourceType_Indirect,
	SourceType_SizeOf,
	SourceType_ArrayElement,
	SourceType_ArrayElementAddress,
	SourceType_ArrayLength,
} SourceType;

typedef union SourceData
{
	struct LocalVariable
	{
		uint32_t LocalVariableIndex;
	} LocalVariable;
	struct LocalVariableAddress
	{
		uint32_t LocalVariableIndex;
	} LocalVariableAddress;
	struct Parameter
	{
		uint32_t ParameterIndex;
	} Parameter;
	struct ParameterAddress
	{
		uint32_t ParameterIndex;
	} ParameterAddress;
	struct ConstantI4
	{
		uint32_t Value;
	} ConstantI4;
	struct ConstantI8
	{
		uint64_t Value;
	} ConstantI8;
	struct ConstantR4
	{
		uint32_t Value;
	} ConstantR4;
	struct ConstantR8
	{
		uint64_t Value;
	} ConstantR8;
	struct Field
	{
		uint32_t FieldIndex;
		IRType* ParentType;
		SourceTypeData* FieldSource;
	} Field;
	struct FieldAddress
	{
		uint32_t FieldIndex;
		IRType* ParentType;
		SourceTypeData* FieldSource;
	} FieldAddress;
	struct StaticField
	{
		IRField* Field;
	} StaticField;
	struct StaticFieldAddress
	{
		IRField* Field;
	} StaticFieldAddress;
	struct Indirect
	{
		IRType* Type;
		SourceTypeData* AddressSource;
	} Indirect;
	struct SizeOf
	{
		IRType* Type;
	} SizeOf;
	struct ArrayElement
	{
		IRType* ElementType;
		SourceTypeData* ArraySource;
		SourceTypeData* IndexSource;
		bool_t NoChecksRequired;
	} ArrayElement;
	struct ArrayElementAddress
	{
		IRType* ElementType;
		SourceTypeData* ArraySource;
		SourceTypeData* IndexSource;
		bool_t NoChecksRequired;
	} ArrayElementAddress;
	struct ArrayLength
	{
		SourceTypeData* ArraySource;
	} ArrayLength;
} SourceData;

struct _SourceTypeData
{
	SourceType Type;
	SourceData Data;
} _SourceTypeData;

#define SourceTypeData_Equal(source1, source2) !memcmp(&source1, &source2, sizeof(SourceTypeData))

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
	
	SourceTypeData Source1;
	SourceTypeData Source2;
	/*
		As a note, the Dup instruction uses
		this source as a second destination.
	*/
	SourceTypeData Source3;

	uint32_t SourceArrayLength;
	SourceTypeData* SourceArray;

	SourceTypeData Destination;
};

IRInstruction* IRInstruction_Create(uint32_t pILLocation, IROpcode pOpcode);
IRInstruction* IRInstruction_Copy(IRInstruction* pInstruction);
void IRInstruction_Destroy(IRInstruction* pInstruction);

struct _TypedReference
{
	uint32_t Type;
	void* Value;
	IRType* Klass;
};
