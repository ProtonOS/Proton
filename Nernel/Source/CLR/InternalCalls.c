#include <CLR/InternalCalls/Proton.IO.MemoryIO.h>
#include <CLR/InternalCalls/Proton.IO.PortIO.h>

#include <CLR/InternalCalls.h>
#include <string.h>


void Panic(const char* msg);

void* ResolveInternalCall(MethodDefinition* methodDef)
{
    if (!strcmp(methodDef->TypeDefinition->Namespace, "Proton.IO"))
    {
        if (!strcmp(methodDef->TypeDefinition->Name, "MemoryIO"))
        {
             if (!strcmp(methodDef->Name, "InByte"))
            {
                return (void*)(Proton_IO_MemoryIO_InByte);
            }
            else if (!strcmp(methodDef->Name, "InUShort"))
            {
                return (void*)(Proton_IO_MemoryIO_InUShort);
            }
            else if (!strcmp(methodDef->Name, "InUInt"))
            {
                return (void*)(Proton_IO_MemoryIO_InUInt);
            }
            else if (!strcmp(methodDef->Name, "OutByte"))
            {
                return (void*)(Proton_IO_MemoryIO_OutByte);
            }
            else if (!strcmp(methodDef->Name, "OutUShort"))
            {
                return (void*)(Proton_IO_MemoryIO_OutUShort);
            }
            else if (!strcmp(methodDef->Name, "OutUInt"))
            {
                return (void*)(Proton_IO_MemoryIO_OutUInt);
            }
            else
            {
                goto LookupFailed;
            }
        }
        else if (!strcmp(methodDef->TypeDefinition->Name, "PortIO"))
        {
            if (!strcmp(methodDef->Name, "InByte"))
            {
                return (void*)(Proton_IO_PortIO_InByte);
            }
            else if (!strcmp(methodDef->Name, "InUShort"))
            {
                return (void*)(Proton_IO_PortIO_InUShort);
            }
            else if (!strcmp(methodDef->Name, "InUInt"))
            {
                return (void*)(Proton_IO_PortIO_InUInt);
            }
            else if (!strcmp(methodDef->Name, "OutByte"))
            {
                return (void*)(Proton_IO_PortIO_OutByte);
            }
            else if (!strcmp(methodDef->Name, "OutUShort"))
            {
                return (void*)(Proton_IO_PortIO_OutUShort);
            }
            else if (!strcmp(methodDef->Name, "OutUInt"))
            {
                return (void*)(Proton_IO_PortIO_OutUInt);
            }
            else
            {
                goto LookupFailed;
            }
        }
        else
        {
            goto LookupFailed;
        }
    }
LookupFailed:
    Panic("Unable to resolve internal call!");
    return 0;
}