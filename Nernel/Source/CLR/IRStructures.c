#include <CLR/IRStructures.h>
#include <stdlib.h>


IRAssembly* IRAssembly_Create()
{
    IRAssembly* asmb = (IRAssembly*)calloc(1, sizeof(IRAssembly));
    return asmb;
}

IRMethod* IRMethod_Create()
{
    IRMethod* mth = (IRMethod*)calloc(1, sizeof(IRMethod));
    return mth;
}

IRInstruction* IRInstruction_Create()
{
    IRInstruction* instr = (IRInstruction*)calloc(1, sizeof(IRInstruction));
    instr->Arg1NeedsDisposing = TRUE;
    instr->Arg2NeedsDisposing = TRUE;
    instr->Arg3NeedsDisposing = TRUE;
    return instr;
}

void IRAssembly_AddMethod(IRAssembly* asmb, IRMethod* mth)
{
	asmb->MethodCount++;
	// The reason this is safe to do even if
	// we haven't allocated for it yet, is because
	// realloc acts like a normal malloc if the first
	// argument is NULL.
	asmb->Methods = (IRMethod**)realloc(asmb->Methods, sizeof(IRMethod*) * asmb->MethodCount);
	asmb->Methods[asmb->MethodCount - 1] = mth;
}

void IRMethod_AddInstruction(IRMethod* mth, IRInstruction* instr)
{
	mth->IRCodesCount++;
	// The reason this is safe to do even if
	// we haven't allocated for it yet, is because
	// realloc acts like a normal malloc if the first
	// argument is NULL.
	mth->IRCodes = (IRInstruction**)realloc(mth->IRCodes, sizeof(IRInstruction*) * mth->IRCodesCount);
	mth->IRCodes[mth->IRCodesCount - 1] = instr;
}


// These should only be getting called within this file
void IRAssembly_Destroy(IRAssembly* assembly);
void IRInstruction_Destroy(IRInstruction* instr);
void IRMethod_Destroy(IRMethod* method);


void ILAssembly_Destroy(ILAssembly* assembly)
{
    CLIFile_Destroy(assembly->CLIFile);
    IRAssembly_Destroy(assembly->IRAssembly);
    free(assembly);
}


void IRAssembly_Destroy(IRAssembly* assembly)
{
	for (uint32_t i = 0; i < assembly->MethodCount; i++)
	{
        IRMethod_Destroy(assembly->Methods[i]);
	}
    free(assembly);
}

void IRMethod_Destroy(IRMethod* method)
{
    if (method->AssembledMethod)
        free(method->AssembledMethod);
    for (uint32_t i = 0; i < method->IRCodesCount; i++)
    {
        IRInstruction_Destroy(method->IRCodes[i]);
    }
    if (method->IRCodes)
        free(method->IRCodes);
    free(method);
}

void IRInstruction_Destroy(IRInstruction* instr)
{
    if (instr->Arg1NeedsDisposing && instr->Arg1)
    {
        free(instr->Arg1);
    }
    if (instr->Arg2NeedsDisposing && instr->Arg2)
    {
        free(instr->Arg2);
    }
    if (instr->Arg3NeedsDisposing && instr->Arg3)
    {
        free(instr->Arg3);
    }
    free(instr);
}