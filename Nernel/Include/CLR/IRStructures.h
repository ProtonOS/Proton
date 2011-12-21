#pragma once

#include <CLR/CLIFile.h>

typedef struct _ILAssembly ILAssembly;
typedef struct _IRAssembly IRAssembly;
typedef struct _IRMethod IRMethod;
typedef struct _IRInstruction IRInstruction;

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
	void* AssembledMethod;
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
};

/*
	Represents a single IR instruction.
 */
struct _IRInstruction
{
	uint32_t OpCode;
    bool_t Arg1NeedsDisposing;
	uint8_t* Arg1;
    bool_t Arg2NeedsDisposing;
	uint8_t* Arg2;
    bool_t Arg3NeedsDisposing;
	uint8_t* Arg3;
	bool_t IsTargetOfBranch;
};

IRAssembly* IRAssembly_Create();
IRMethod* IRMethod_Create();
IRInstruction* IRInstruction_Create();

void ILAssembly_Destroy(ILAssembly* assembly);

void IRAssembly_AddMethod(IRAssembly* asmb, IRMethod* mth);
void IRMethod_AddInstruction(IRMethod* mth, IRInstruction* instr);