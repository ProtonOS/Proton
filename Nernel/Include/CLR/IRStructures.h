#pragma once

typedef struct _ILAssembly ILAssembly;
typedef struct _IRAssembly IRAssembly;
typedef struct _IRMethod IRMethod;
typedef struct _IRInstruction IRInstruction;
typedef struct _IRParameter IRParameter;
typedef struct _IRType IRType;
typedef struct _IRLocalVariable IRLocalVariable;
typedef struct _IRField IRField;



// DO NOT put this include above the typedefs,
// the typedefs are needed to be able to resolve
// circular inclusions.
#include <CLR/CLIFile.h>
#include <CLR/OpCodes_IR.h>

struct _ILAssembly
{
    CLIFile* CLIFile;
    IRAssembly* IRAssembly;
};

struct _IRAssembly
{
    uint32_t MethodCount;
    IRMethod** Methods;
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
    //void* AssembledMethod;
    void(*AssembledMethod)();
	/*
		The parent assembly for this method.
	 */
	IRAssembly* ParentAssembly;
	/*
		
	 */
	uint32_t IRCodesCount;
	/*
		
	 */
	IRInstruction** IRCodes;
	/*
		
	 */
	uint32_t ParametersCount;
	/*
		
	 */
	IRParameter* Parameters;
	
	
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
    bool_t IsTargetOfBranch;
};

struct _IRParameter
{
    bool_t IsIn;
    bool_t IsOut;
    IRType* Type;
    bool_t HasDefault;
    void* DefaultValue;
};

struct _IRType
{
    bool_t IsValueType;
    bool_t IsReferenceType;
    bool_t IsStatic;
    uint32_t FieldCount;
    IRField** Fields;
    
};

struct _IRLocalVariable
{
    IRType* VariableType;
    void* Value;
};

struct _IRField
{
    IRType* FieldType;
    void* Value;
};


IRAssembly* IRAssembly_Create();
IRMethod* IRMethod_Create();
IRInstruction* IRInstruction_Create();


void ILAssembly_Destroy(ILAssembly* assembly);

void IRAssembly_AddMethod(IRAssembly* asmb, IRMethod* mth);
void IRMethod_AddInstruction(IRMethod* mth, IRInstruction* instr);