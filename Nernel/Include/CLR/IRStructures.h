#pragma once

typedef struct _IRAssembly IRAssembly;
typedef struct _IRType IRType;
typedef struct _IRMethod IRMethod;
typedef struct _IRInstruction IRInstruction;
typedef struct _IRParameter IRParameter;
typedef struct _IRLocalVariable IRLocalVariable;
typedef struct _IRField IRField;
typedef struct _IRMethodSpec IRMethodSpec;
typedef struct _IRFieldSpec IRFieldSpec;
typedef struct _IRArrayType IRArrayType;
typedef struct _IRInterfaceImpl IRInterfaceImpl;


// DO NOT put this include above the typedefs,
// the typedefs are needed to be able to resolve
// circular inclusions.
#include <uthash.h>
#include <CLR/CLIFile.h>
#include <CLR/OpCodes_IR.h>
#include <CLR/AppDomain.h>

// Also, please DO NOT modify the
// structures unless you first check
// very thoroughly with the JIT, because
// certain portions of the JIT require things
// to be in certain places.

struct _IRAssembly
{
	/*
        The index in the AppDomain's assembly array.
     */
	uint32_t AssemblyIndex;
	AppDomain* ParentDomain;

	IRMethod* EntryPoint;

    uint32_t MethodCount;
    IRMethod** Methods;
    uint32_t FieldCount;
    IRField** Fields;
    uint32_t TypeCount;
    IRType** Types;

	IRArrayType* ArrayTypesHashTable;

	CLIFile* ParentFile;
};

struct _IRType
{
	uint32_t TypeIndex;
	IRAssembly* ParentAssembly;

    bool_t IsValueType;
    bool_t IsReferenceType;
	bool_t IsGeneric;
	bool_t IsInterface;
	bool_t IsVoid;
    
	bool_t HasStaticConstructor;
	IRMethod* StaticConstructor;

	uint32_t MethodCount;
	IRMethod** Methods;

    uint32_t FieldCount;
    IRField** Fields;
	bool_t FieldsLayedOut;

	bool_t IsFixedSize;
	uint32_t Size;

	TypeDefinition* TypeDef;

	bool_t HasFinalizer;
	IRMethod* Finalizer;

	IRArrayType* ArrayType;


	IRInterfaceImpl* InterfaceTable;
};

struct _IRInterfaceImpl
{
	IRType* InterfaceType;

	uint32_t MethodCount;
	uint32_t* MethodIndexes;

	UT_hash_handle HashHandle;
};

struct _IRArrayType
{
	IRType* ArrayElementType;
	IRType* ArrayType;
	UT_hash_handle HashHandle;
};

struct _IRMethod
{
    /*
        The index in the IRAssembly's method array.
     */
    uint32_t MethodIndex;

    /*
        The actual assembled method.
     */
    void(*AssembledMethod)();
	/*
		The parent assembly for this method.
	 */
	IRAssembly* ParentAssembly;

	uint32_t IRCodesCount;
	IRInstruction** IRCodes;

	uint32_t ParameterCount;
	IRParameter** Parameters;

	uint32_t LocalVariableCount;
	IRLocalVariable** LocalVariables;	

	MethodDefinition* MethodDefinition;

	bool_t Returns;
	IRType* ReturnType;

	bool_t ParametersLayedOut;
	bool_t LocalsLayedOut;
};

/*
	Represents a single IR instruction.
 */
struct _IRInstruction
{
    uint32_t InstructionLocation;
    IROpCode OpCode;
    bool_t Arg1NeedsDisposing;
    void* Arg1;
    bool_t Arg2NeedsDisposing;
    void* Arg2;
    bool_t Arg3NeedsDisposing;
    void* Arg3;
    bool_t Arg4NeedsDisposing;
    void* Arg4;
    bool_t IsTargetOfBranch;
};

struct _IRParameter
{
	uint32_t ParameterIndex;
    IRType* Type;
	uint32_t Offset;
	uint32_t Size;
};


struct _IRLocalVariable
{
	uint32_t LocalVariableIndex;
    IRType* VariableType;
	uint32_t Offset;
	uint32_t Size;
};

struct _IRField
{
	uint32_t FieldIndex;
	IRType* ParentType;
    IRType* FieldType;
	uint32_t Offset;
	uint32_t Size;

	Field* FieldDef;

	bool_t IsStatic;
	void* StaticValue;

	IRAssembly* ParentAssembly;
};

struct _IRMethodSpec
{
	/*
		The parent type of this method.
	 */
	IRType* ParentType;
	/*
		The index in the parent type's method list.
	 */
	uint32_t MethodIndex;
};

struct _IRFieldSpec
{
	/*
		The type of this field.
	 */
	IRType* FieldType;
	/*
		The index in the parent type's field list.
	 */
	uint32_t FieldIndex;
	/*
		The type that this field is in.
	 */
	IRType* ParentType;
};

IRAssembly* IRAssembly_Create(AppDomain* domain);
IRMethod* IRMethod_Create();
IRInstruction* IRInstruction_Create();
IRParameter* IRParameter_Create();
IRType* IRType_Create();
IRArrayType* IRArrayType_Create(IRType* pElementType, IRType* pArrayType);
IRLocalVariable* IRLocalVariable_Create();
IRField* IRField_Create();
IRMethodSpec* IRMethodSpec_Create();
IRFieldSpec* IRFieldSpec_Create();


void IRAssembly_Destroy(IRAssembly* asmb);
void IRMethod_Destroy(IRMethod* mth);
void IRInstruction_Destroy(IRInstruction* instr);
void IRParameter_Destroy(IRParameter* param);
void IRType_Destroy(IRType* tp);
void IRArrayType_Destroy(IRArrayType* tp);
void IRLocalVariable_Destroy(IRLocalVariable* var);
void IRField_Destroy(IRField* fld);
void IRMethodSpec_Destroy(IRMethodSpec* mthspec);
void IRFieldSpec_Destroy(IRFieldSpec* mthspec);


void IRAssembly_AddMethod(IRAssembly* asmb, IRMethod* mth);
void IRAssembly_AddType(IRAssembly* asmb, IRType* tp);
void IRAssembly_AddField(IRAssembly* asmb, IRField* fld);

void IRMethod_AddInstruction(IRMethod* mth, IRInstruction* instr);
void IRMethod_AddLocalVariable(IRMethod* mth, IRLocalVariable* var);
void IRMethod_AddParameter(IRMethod* mth, IRParameter* param);

void IRType_AddField(IRType* tp, IRField* fld);

uint32_t IRType_GetSize(IRType* tp);