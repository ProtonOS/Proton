#pragma once

#include "../uthash.h"
#include "Metadata.h"
#include "IROpcode.h"

namespace CLR
{
	class Assembly;
	class IRGenericParameter;
	class IRType;
	class IRMethod;
	class IRField;
	class IRArrayType;
	class IRPointerType;
	class IRInterfaceImpl;
	class IRGenericType;
	class IRParameter;
	class IRLocalVariable;
	class IRInstruction;
	class IRGenericMethod;
	class IRLocalSSAData;
	class SourceTypeData;
	class IRInstruction;


	class IRGenericParameter
	{
	public:
		bool FromParentType;
		uint32_t Index;

		IRGenericParameter();
		~IRGenericParameter();
	};

	class IRType
	{
	public:
		uint32_t TypeIndex;

		Assembly* ParentAssembly;
		TypeDefinitionData* TypeDefinition;

		bool IsValueType : 1;
		bool IsReferenceType : 1;
		bool IsArrayType : 1;
		bool IsPointerType : 1;
		bool IsGeneric : 1;
		bool IsGenericInstantiation : 1;
		bool IsInterface : 1;
		bool IsAbstract : 1;
    
		bool HasStaticConstructor;
		bool StaticConstructorCalled;
		IRMethod* StaticConstructor;

		uint32_t MethodCount;
		IRMethod** Methods;

		uint32_t FieldCount;
		IRField** Fields;
		bool FieldsLayedOut;

		bool StackSizeCalculated;
		uint32_t StackSize;
		bool SizeCalculated;
		uint32_t Size;

		bool HasFinalizer;
		IRMethod* Finalizer;
	
		IRArrayType* ArrayType;
		IRPointerType* PointerType;

		IRInterfaceImpl* InterfaceTable;

		IRGenericType* GenericType;

		uint32_t GenericParameterCount;

		bool IsGenericParameter;
		bool IsGenericParameterFromParentType;
		uint32_t GenericParameterIndex;

		IRType();
		~IRType();

		void LayoutMethods();
		void LayoutFields();

		IRType* Copy();
		IRType* GenericDeepCopy();
	};

	class IRMethod
	{
	public:
		uint32_t MethodIndex;

		Assembly* ParentAssembly;
		MethodDefinitionData* MethodDefinition;

		bool Returns;
		IRType* ReturnType;

		uint32_t ParameterCount;
		IRParameter** Parameters;
		bool ParametersLayedOut;

		uint32_t LocalVariableCount;
		IRLocalVariable** LocalVariables;
		bool LocalsLayedOut;

		uint32_t IRCodesCount;
		IRInstruction** IRCodes;

		bool IsGeneric;
		bool IsGenericImplementation;
		uint32_t GenericParameterCount;
		IRGenericMethod* GenericMethod;

		bool IsDomainSpecific;

		void(*AssembledMethod)();

		IRMethod();
		~IRMethod();

		bool UsesGenerics();
	};

	class IRParameter
	{
	public:
		uint32_t ParameterIndex;

		IRType* Type;
		uint32_t Offset;
		uint32_t Size;

		IRParameter(IRType* pType);
		~IRParameter();
	};

	class IRLocalVariable
	{
	public:
		uint32_t LocalVariableIndex;
		IRType* VariableType;
		uint32_t Offset;
		uint32_t Size;
		bool Pinned;

		IRLocalSSAData* SSAData;

		IRLocalVariable();
		~IRLocalVariable();
	};

	class IRLocalSSAData
	{
	public:
		IRLocalVariable* Derived;
		uint32_t Iteration;
		IRInstruction* LifeStarted;
		IRInstruction* LifeEnded;

		IRLocalSSAData();
		~IRLocalSSAData();
	};

	class IRField
	{
	public:
		uint32_t FieldIndex;

		Assembly* ParentAssembly;
		FieldData* FieldDefinition;

		IRType* ParentType;
		IRType* FieldType;

		uint32_t Offset;
		uint32_t Size;

		bool IsStatic;
		uint32_t StaticFieldIndex;

		IRField();
		~IRField();
	};

	class IRArrayType
	{
	public:
		IRType* ArrayType;
		IRType* ElementType;
		UT_hash_handle HashHandle;

		IRArrayType(IRType* pArrayType, IRType* pElementType);
		~IRArrayType();
	};

	class IRPointerType
	{
	public:
		IRType* PointerType;
		IRType* TypePointedTo;
		UT_hash_handle HashHandle;

		IRPointerType(IRType* pPointerType, IRType* pPointedToType);
		~IRPointerType();
	};

	class IRInterfaceImpl
	{
	public:
		IRType* InterfaceType;

		uint32_t MethodCount;
		uint32_t* MethodIndexes;

		UT_hash_handle HashHandle;

		IRInterfaceImpl(IRType* pType);
		~IRInterfaceImpl();
	};

	class IRGenericType
	{
	public:
		const static uint8_t MAX_PARAMETERS = 32;

		IRType* GenericType;

		uint32_t ParameterCount;
		IRType* Parameters[MAX_PARAMETERS];

		// This must come after the other parts, for keying reasons
		IRType* ImplementationType;

		UT_hash_handle HashHandle;

		IRGenericType();
		IRGenericType(IRType* pGenericType, IRType* pImplementationType);
		~IRGenericType();
	};

	class IRGenericMethod
	{
	public:
		const static uint8_t MAX_PARAMETERS = 32;

		IRType* ParentType;
		IRMethod* GenericMethod;

		uint32_t ParameterCount;
		IRType* Parameters[MAX_PARAMETERS];

		// This must come after the other parts, for keying reasons
		IRMethod* ImplementationMethod;

		UT_hash_handle HashHandle;

		IRGenericMethod();
		~IRGenericMethod();
	};

	namespace SourceType
	{
		typedef enum SourceType
		{
			Null,
			Local,
			LocalAddress,
			Parameter,
			ParameterAddress,
			ConstantI4,
			ConstantI8,
			ConstantR4,
			ConstantR8,
			Field,
			FieldAddress,
			StaticField,
			StaticFieldAddress,
			Indirect,
			SizeOf,
			ArrayElement,
			ArrayElementAddress,
			ArrayLength,
		} SourceType;
	};

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
		} ArrayElement;
		struct ArrayElementAddress
		{
			IRType* ElementType;
			SourceTypeData* ArraySource;
			SourceTypeData* IndexSource;
		} ArrayElementAddress;
		struct ArrayLength
		{
			SourceTypeData* ArraySource;
		} ArrayLength;
	} SourceData;

	class SourceTypeData
	{
	public:
		SourceType::SourceType Type;
		SourceData Data;

		SourceTypeData();
		~SourceTypeData();
	};

	class IRInstruction
	{
	public:
		uint32_t ILLocation;
		uint32_t IRLocation;
		IROpcode::IROpcode Opcode;
		bool Arg1NeedsDisposing;
		void* Arg1;
		bool Arg2NeedsDisposing;
		void* Arg2;
		bool Arg3NeedsDisposing;
		void* Arg3;
		bool Arg4NeedsDisposing;
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

		IRInstruction();
		~IRInstruction();
	};
};
