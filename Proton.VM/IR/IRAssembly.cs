using Proton.Metadata;
using Proton.Metadata.Tables;
using Proton.Metadata.Signatures;
using System;
using System.Linq;
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
            foreach (MethodDefData methodDefData in File.MethodDefTable) Methods.Add(new IRMethod(this));

            for (int typeIndex = 0; typeIndex < Types.Count; ++typeIndex)
            {
                IRType type = Types[typeIndex];
                TypeDefData typeDefData = File.TypeDefTable[typeIndex];

                type.Namespace = typeDefData.TypeNamespace;
                type.Name = typeDefData.TypeName;
				var genParams = File.GenericParamTable.Where(gp => gp.Owner.Type == TypeOrMethodDefIndex.TypeOrMethodDefType.TypeDef && gp.Owner.TypeDef == typeDefData).ToList();
				for (int i = 0; i < genParams.Count; i++)
				{
					type.GenericParameters.Add(IRType.GetVarPlaceholder(genParams[i].Number));
				}

                foreach (FieldData fieldData in typeDefData.FieldList)
                {
                    IRField field = Fields[fieldData.TableIndex];
                    field.Name = fieldData.Name;
                    field.ParentType = type;
                    type.Fields.Add(field);
                }
                foreach (MethodDefData methodDefData in typeDefData.MethodList)
                {
                    IRMethod method = Methods[methodDefData.TableIndex];
                    method.Name = methodDefData.Name;
                    method.ParentType = type;
					method.IsStatic = (methodDefData.Flags & MethodAttributes.Static) == MethodAttributes.Static;
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
                            local.Index = (uint)method.Locals.Count;
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
                    nestedType.Namespace = type.Namespace + "." + type.Name;
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
            }
            for (int methodIndex = 0; methodIndex < Methods.Count; ++methodIndex)
            {
                IRMethod method = Methods[methodIndex];
				MethodDefData methodDefData = File.MethodDefTable[methodIndex];
				var mGenParams = File.GenericParamTable.Where(gp => gp.Owner.Type == TypeOrMethodDefIndex.TypeOrMethodDefType.MethodDef && gp.Owner.MethodDef == methodDefData).ToList();
				for (int i = 0; i < mGenParams.Count; i++)
				{
					method.GenericParameters.Add(IRType.GetMVarPlaceholder(mGenParams[i].Number));
				}
                method.ReturnType = AppDomain.PresolveType(methodDefData.ExpandedSignature.RetType);
                for (int parameterIndex = 0; parameterIndex < method.Parameters.Count; ++parameterIndex)
                {
                    IRParameter parameter = method.Parameters[parameterIndex];
                    parameter.Type = AppDomain.PresolveType(methodDefData.ExpandedSignature.Params[parameterIndex]);
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
                    implicitThis.Type = method.ParentType.IsValueType ? AppDomain.GetPointerType(method.ParentType) : method.ParentType;
                    method.Parameters.Insert(0, implicitThis);
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

        internal void LoadStage5()
        {
			Console.WriteLine("========== Stage 5: {0,-45} ==========", File.ReferenceName);
		}

		internal void LoadStage6()
		{
			Console.WriteLine("========== Stage 6: {0,-45} ==========", File.ReferenceName);
			// Optimizations
			AppDomain.Methods.ForEach(m => m.ControlFlowGraph = IRControlFlowGraph.Build(m));
			AppDomain.Methods.ForEach(m => m.EnterSSA());
			AppDomain.Methods.ForEach(m => m.LeaveSSA());
		}
    }
}
