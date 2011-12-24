#pragma once

#define ReferenceTypeObject_Flags_Pinned            0x01
#define ReferenceTypeObject_Flags_Active            0x80

typedef struct
{
    uint8_t Flags;
    uint32_t ReferenceCount;
    uint32_t Size;
    uint8_t* Object;
} ReferenceTypeObject;