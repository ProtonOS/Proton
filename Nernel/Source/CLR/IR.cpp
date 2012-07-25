#include "IR.h"

CLR::IRGenericParameter::IRGenericParameter()
{
}

CLR::IRGenericParameter::~IRGenericParameter()
{
}

CLR::IRType::IRType()
{
}

CLR::IRType::~IRType()
{
}

void CLR::IRType::LayoutMethods()
{
}

void CLR::IRType::LayoutFields()
{
}

CLR::IRType* CLR::IRType::Copy()
{
	return nullptr;
}

CLR::IRType* CLR::IRType::GenericDeepCopy()
{
	return nullptr;
}

CLR::IRMethod::IRMethod()
{
}

CLR::IRMethod::~IRMethod()
{
}

bool CLR::IRMethod::UsesGenerics()
{
	return false;
}

CLR::IRParameter::IRParameter(IRType* pType)
{
}

CLR::IRParameter::~IRParameter()
{
}

CLR::IRLocalVariable::IRLocalVariable()
{
}

CLR::IRLocalVariable::~IRLocalVariable()
{
}

CLR::IRLocalSSAData::IRLocalSSAData()
{
}

CLR::IRLocalSSAData::~IRLocalSSAData()
{
}

CLR::IRField::IRField()
{
}

CLR::IRField::~IRField()
{
}

CLR::IRArrayType::IRArrayType(IRType* pArrayType, IRType* pElementType)
{
}

CLR::IRArrayType::~IRArrayType()
{
}

CLR::IRPointerType::IRPointerType(IRType* pPointerType, IRType* pPointedToType)
{
}

CLR::IRPointerType::~IRPointerType()
{
}

CLR::IRInterfaceImpl::IRInterfaceImpl(IRType* pType)
{
}

CLR::IRInterfaceImpl::~IRInterfaceImpl()
{
}

CLR::IRGenericType::IRGenericType()
{
}

CLR::IRGenericType::IRGenericType(IRType* pGenericType, IRType* pImplementationType)
{
}

CLR::IRGenericType::~IRGenericType()
{
}

CLR::IRGenericMethod::IRGenericMethod()
{
}

CLR::IRGenericMethod::~IRGenericMethod()
{
}

CLR::SourceTypeData::SourceTypeData()
{
}

CLR::SourceTypeData::~SourceTypeData()
{
}

CLR::IRInstruction::IRInstruction()
{
}

CLR::IRInstruction::~IRInstruction()
{
}
