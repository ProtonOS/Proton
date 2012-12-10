﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Proton.Metadata.Signatures
{
    public enum SigElementType : byte
    {
	    End							    = 0x00,
	    Void							= 0x01,
	    Boolean						    = 0x02,
	    Char							= 0x03,
	    I1								= 0x04,
	    U1								= 0x05,
	    I2								= 0x06,
	    U2								= 0x07,
	    I4								= 0x08,
	    U4								= 0x09,
	    I8								= 0x0A,
	    U8								= 0x0B,
	    R4								= 0x0C,
	    R8								= 0x0D,
	    String							= 0x0E,
	    Pointer						    = 0x0F,
	    ByReference					    = 0x10,
	    ValueType						= 0x11,
	    Class							= 0x12,
	    Var							    = 0x13,
	    Array							= 0x14,
	    GenericInstantiation			= 0x15,
	    TypedByReference				= 0x16,
	    IPointer						= 0x18,
	    UPointer						= 0x19,
	    FunctionPointer				    = 0x1B,
	    Object							= 0x1C,
	    SingleDimensionArray			= 0x1D,
	    MethodVar						= 0x1E,
	    CustomModifier_Required		    = 0x1F,
	    CustomModifier_Optional		    = 0x20,
	    Internal						= 0x21,
	    Modifier						= 0x40,
	    Sentinel						= 0x41,
	    Pinned							= 0x45,
	    Type							= 0x50,
	    CustomAttribute_Boxed			= 0x51,
	    CustomAttribute_Field			= 0x53,
	    CustomAttribute_Property		= 0x54,
	    CustomAttribute_Enum			= 0x55,
    }
}