#include <CLR/Internal.h>
#include <CLR/Internal/Proton.IO.PortIO.h>


const InternalCall InternalCallTable[] = 
{
	{	"Proton.IO",		"PortIO",			"InByte",				SignatureElementType_U1,		1,	{ SignatureElementType_U4 }, &Proton_IO_PortIO_InByte },
	{	NULL,				NULL,				"InUShort",				SignatureElementType_U2,		1,	{ SignatureElementType_U4 }, &Proton_IO_PortIO_InUShort },
	{	NULL,				NULL,				"InUInt",				SignatureElementType_U4,		1,	{ SignatureElementType_U4 }, &Proton_IO_PortIO_InUInt },
	{	NULL,				NULL,				"OutByte",				SignatureElementType_Void,		2,	{ SignatureElementType_U4, SignatureElementType_U1 }, &Proton_IO_PortIO_OutByte },
	{	NULL,				NULL,				"OutUShort",			SignatureElementType_Void,		2,	{ SignatureElementType_U4, SignatureElementType_U2 }, &Proton_IO_PortIO_OutUShort },
	{	NULL,				NULL,				"OutUInt",				SignatureElementType_Void,		2,	{ SignatureElementType_U4, SignatureElementType_U4 }, &Proton_IO_PortIO_OutUInt },


	{	NULL,				NULL,				NULL,					SignatureElementType_End,		0,	{ }, NULL }
};
