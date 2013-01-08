using Proton.Metadata;
using Proton.Metadata.Tables;
using Proton.Metadata.Signatures;
using System;
using System.Collections.Generic;

namespace Proton.VM.IR
{
	public sealed class IRAssembly
	{
		public IRAppDomain AppDomain = null;
		public CLIFile File = null;
		public bool CORLibrary = false;
		public List<IRType> Types = new List<IRType>();
		public List<IRField> Fields = new List<IRField>();
		public List<IRMethod> Methods = new List<IRMethod>();

		internal IRAssembly(IRAppDomain pAppDomain, CLIFile pCLIFile, bool pCORLibrary)
		{
			AppDomain = pAppDomain;
			File = pCLIFile;
			CORLibrary = pCORLibrary;
		}

		internal void LoadStage1()
		{
			Console.WriteLine("========== Stage 1: {0,-45} ==========", File.ReferenceName);
			foreach (TypeDefData typeDefData in File.TypeDefTable) Types.Add(new IRType(this));
			foreach (FieldData fieldData in File.FieldTable) Fields.Add(new IRField(this));
			foreach (MethodDefData methodDefData in File.MethodDefTable)
			{
				IRMethod method = new IRMethod(this);
				Methods.Add(method);
				var mGenParams = new List<GenericParamData>(Array.FindAll(File.GenericParamTable, gp => gp.Owner.Type == TypeOrMethodDefIndex.TypeOrMethodDefType.MethodDef && gp.Owner.MethodDef == methodDefData));
				for (int i = 0; i < mGenParams.Count; i++)
				{
					method.GenericParameters.Add(IRType.GetMVarPlaceholder(mGenParams[i].Number));
				}
			}

			for (int typeIndex = 0; typeIndex < Types.Count; ++typeIndex)
			{
				IRType type = Types[typeIndex];
				TypeDefData typeDefData = File.TypeDefTable[typeIndex];

				type.Namespace = typeDefData.TypeNamespace;
				type.Name = typeDefData.TypeName;
				type.Flags = typeDefData.Flags;
				var genParams = new List<GenericParamData>(Array.FindAll(File.GenericParamTable, gp => gp.Owner.Type == TypeOrMethodDefIndex.TypeOrMethodDefType.TypeDef && gp.Owner.TypeDef == typeDefData));
				for (int i = 0; i < genParams.Count; i++)
				{
					type.GenericParameters.Add(IRType.GetVarPlaceholder(genParams[i].Number));
				}

				foreach (FieldData fieldData in typeDefData.FieldList)
				{
					IRField field = Fields[fieldData.TableIndex];
					field.Name = fieldData.Name;
					field.Flags = fieldData.Flags;
					field.ParentType = type;
					type.Fields.Add(field);
				}
				foreach (MethodDefData methodDefData in typeDefData.MethodList)
				{
					IRMethod method = Methods[methodDefData.TableIndex];
					method.Name = methodDefData.Name;
					method.Flags = methodDefData.Flags;
					method.ImplFlags = methodDefData.ImplFlags;
					method.ParentType = type;
					type.Methods.Add(method);

					foreach (ParamData paramData in methodDefData.ParamList)
					{
						IRParameter parameter = new IRParameter(this);
						parameter.ParentMethod = method;
						method.Parameters.Add(parameter);
					}

					if (methodDefData.Body != null && methodDefData.Body.ExpandedLocalVarSignature != null)
					{
						method.MaximumStackDepth = methodDefData.Body.MaxStack;
						foreach (SigLocalVar sigLocalVar in methodDefData.Body.ExpandedLocalVarSignature.LocalVars)
						{
							IRLocal local = new IRLocal(this);
							local.ParentMethod = method;
							local.Index = method.Locals.Count;
							method.Locals.Add(local);
						}
					}
				}
			}
			for (int typeIndex = 0; typeIndex < Types.Count; ++typeIndex)
			{
				IRType type = Types[typeIndex];
				TypeDefData typeDefData = File.TypeDefTable[typeIndex];

				foreach (TypeDefData nestedTypeDefData in typeDefData.NestedClassList)
				{
					IRType nestedType = Types[nestedTypeDefData.TableIndex];
					//nestedType.Namespace = type.Namespace + "." + type.Name;
					nestedType.NestedInsideOfType = type;
					type.NestedTypes.Add(nestedType);
				}
			}
			if (CORLibrary) AppDomain.CacheCORTypes(this);
		}

		internal void LoadStage2()
		{
			Console.WriteLine("========== Stage 2: {0,-45} ==========", File.ReferenceName);

			for (int typeIndex = 0; typeIndex < Types.Count; ++typeIndex)
			{
				IRType type = Types[typeIndex];
				TypeDefData typeDefData = File.TypeDefTable[typeIndex];
				if (typeDefData.Extends.Type != TypeDefRefOrSpecIndex.TypeDefRefOrSpecType.TypeDef || typeDefData.Extends.TypeDef != null) type.BaseType = AppDomain.PresolveType(typeDefData.Extends);
				for (int fieldIndex = 0; fieldIndex < type.Fields.Count; ++fieldIndex)
				{
					IRField field = type.Fields[fieldIndex];
					field.Type = AppDomain.PresolveType(typeDefData.FieldList[fieldIndex].ExpandedSignature);
					if (field.Type == null) throw new Exception();
				}
				for (int interfaceIndex = 0; interfaceIndex < typeDefData.InterfaceList.Count; ++interfaceIndex)
				{
					type.ImplementedInterfaces.Add(AppDomain.PresolveType(typeDefData.InterfaceList[interfaceIndex]));
				}
			}
			for (int fieldIndex = 0; fieldIndex < Fields.Count; ++fieldIndex)
			{
				IRField field = Fields[fieldIndex];
				if (field.IsLiteral)
				{
					ConstantData constantData = Array.Find(File.ConstantTable, c => c.Parent.Type == HasConstantIndex.HasConstantType.Field && c.Parent.Field.TableIndex == fieldIndex);
					if (constantData == null) throw new Exception();
					field.LiteralType = constantData.Type;
					field.LiteralValue = constantData.Value;
				}
			}
			for (int layoutIndex = 0; layoutIndex < File.ClassLayoutTable.Length; ++layoutIndex)
			{
				ClassLayoutData classLayoutData = File.ClassLayoutTable[layoutIndex];
				IRType type = Types[classLayoutData.Parent.TableIndex];
				type.ClassSize = (int)classLayoutData.ClassSize;
				type.PackingSize = (int)classLayoutData.PackingSize;
			}
			for (int i = 0; i < File.FieldLayoutTable.Length; i++)
			{
				FieldLayoutData fieldLayoutData = File.FieldLayoutTable[i];
				IRField field = Fields[fieldLayoutData.Field.TableIndex];
				field.Offset = (int)fieldLayoutData.Offset;
			}
			for (int methodIndex = 0; methodIndex < Methods.Count; ++methodIndex)
			{
				IRMethod method = Methods[methodIndex];
				MethodDefData methodDefData = File.MethodDefTable[methodIndex];

				method.ReturnType = AppDomain.PresolveType(methodDefData.ExpandedSignature.RetType);
				int missingFromSig = 0;
				for (int parameterIndex = 0; parameterIndex < method.Parameters.Count; ++parameterIndex)
				{
					IRParameter parameter = method.Parameters[parameterIndex];
					if ((methodDefData.ParamList[parameterIndex].Flags & ParamAttributes.HasFieldMarshal) == ParamAttributes.HasFieldMarshal)
					{
						++missingFromSig;
						parameter.Type = AppDomain.System_IntPtr;
					}
					else
						parameter.Type = AppDomain.PresolveType(methodDefData.ExpandedSignature.Params[parameterIndex - missingFromSig]);
				}
				for (int localIndex = 0; localIndex < method.Locals.Count; ++localIndex)
				{
					IRLocal local = method.Locals[localIndex];
					local.Type = AppDomain.PresolveType(methodDefData.Body.ExpandedLocalVarSignature.LocalVars[localIndex]);
				}
			}
			for (int methodIndex = 0; methodIndex < Methods.Count; ++methodIndex)
			{
				IRMethod method = Methods[methodIndex];
				MethodDefData methodDefData = File.MethodDefTable[methodIndex];
				if (methodDefData.ExpandedSignature.HasThis && !methodDefData.ExpandedSignature.ExplicitThis)
				{
					IRParameter implicitThis = new IRParameter(this);
					implicitThis.ParentMethod = method;
					implicitThis.Type = method.ParentType.IsValueType ? AppDomain.GetUnmanagedPointerType(method.ParentType) : method.ParentType;
					method.Parameters.Insert(0, implicitThis);
				}
			}
			for (int typeIndex = 0; typeIndex < Types.Count; ++typeIndex)
			{
				IRType type = Types[typeIndex];
				TypeDefData typeDefData = File.TypeDefTable[typeIndex];
				for (int methodImplIndex = 0; methodImplIndex < File.MethodImplTable.Length; ++methodImplIndex)
				{
					if (File.MethodImplTable[methodImplIndex].Class == typeDefData)
					{
						type.ExplicitOverrides.Add(new IRType.OverrideDescriptor(AppDomain.PresolveMethod(File.MethodImplTable[methodImplIndex].MethodDeclaration), AppDomain.PresolveMethod(File.MethodImplTable[methodImplIndex].MethodBody)));
					}
				}
			}
		}

		internal void LoadStage3()
		{
			Console.WriteLine("========== Stage 3: {0,-45} ==========", File.ReferenceName);
			for (int methodIndex = 0; methodIndex < Methods.Count; ++methodIndex)
			{
				Methods[methodIndex].ConvertInstructions(File.MethodDefTable[methodIndex]);
				Methods[methodIndex].LinearizeInstructions(File.MethodDefTable[methodIndex]);
				Methods[methodIndex].TransformInstructions(File.MethodDefTable[methodIndex]);
			}
		}

		internal void LoadStage4()
		{
			Console.WriteLine("========== Stage 4: {0,-45} ==========", File.ReferenceName);
			// Generic resolution and type updating
			Types.ForEach(t => t.Substitute(t.GenericParameters, IRGenericParameterList.Empty));
		}



		public void Dump(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("IRAssembly {0}", File.ReferenceName);
			pWriter.WriteLine("{");
			pWriter.Indent++;
			Types.ForEach(t => t.Dump(pWriter));
			pWriter.Indent--;
			pWriter.WriteLine("}");
		}
	}
}
