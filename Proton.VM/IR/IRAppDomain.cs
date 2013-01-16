//#define ProfileMemory
#define ProfileSpeed
using Proton.Metadata;
using Proton.Metadata.Tables;
using Proton.Metadata.Signatures;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using Proton.VM.IR.Optimizations;
using Proton.VM.IR.Transformations;
using System.Diagnostics;

namespace Proton.VM.IR
{
	public sealed class IRAppDomain
	{
		// This class should ONLY ever be used here.
		public sealed class IRTypeCollection : IEnumerable<IRType>
		{
			private readonly List<IRType> vals = new List<IRType>(4096);

			public int Count { get { return vals.Count; } }
			public IRType this[int idx] { get { return vals[idx]; } }

			public void Add(IRType t)
			{
				if (!t.Resolved)
					throw new Exception("You cannot add a type which isn't fully resolved to the global type collection!");
				vals.Add(t);
				t.GlobalTypeID = vals.Count - 1;
			}
			
			public List<IRType> FindAll(Predicate<IRType> cond)
			{
				return vals.FindAll(cond);
			}

			public IEnumerator<IRType> GetEnumerator()
			{
				foreach (IRType t in vals)
					yield return t;
			}

			System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
			{
				foreach (IRType t in vals)
					yield return t;
			}
		}
		public IRTypeCollection Types = new IRTypeCollection();

		public sealed class IRMethodCollection : IEnumerable<IRMethod>
		{
			private readonly List<IRMethod> vals = new List<IRMethod>(8182);

			public int Count { get { return vals.Count; } }
			public IRMethod this[int idx] { get { return vals[idx]; } }

			public void Add(IRMethod m)
			{
				if (!m.Resolved)
					throw new Exception("You cannot add a method which isn't fully resolved to the global method collection!");
				vals.Add(m);
				m.GlobalMethodID = vals.Count - 1;
			}

			public List<IRMethod> FindAll(Predicate<IRMethod> cond)
			{
				return vals.FindAll(cond);
			}

			public IEnumerator<IRMethod> GetEnumerator()
			{
				foreach (IRMethod m in vals)
					yield return m;
			}

			System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
			{
				foreach (IRMethod m in vals)
					yield return m;
			}
		}
		public IRMethodCollection Methods = new IRMethodCollection();

		public List<IRField> StaticFields = new List<IRField>();


		public List<IRAssembly> Assemblies = new List<IRAssembly>();
		public Dictionary<CLIFile, IRAssembly> AssemblyFileLookup = new Dictionary<CLIFile, IRAssembly>();
		public Dictionary<string, IRAssembly> AssemblyFileReferenceNameLookup = new Dictionary<string, IRAssembly>();

		public IRType System_Array = null;
		public IRType System_Boolean = null;
		public IRType System_Byte = null;
		public IRType System_Char = null;
		public IRType System_Double = null;
		public IRType System_Enum = null;
		public IRType System_Exception = null;
		public IRType System_GC = null;
		public IRType System_Int16 = null;
		public IRType System_Int32 = null;
		public IRType System_Int64 = null;
		public IRType System_IntPtr = null;
		public IRType System_Object = null;
		public IRType System_RuntimeArgumentHandle = null;
		public IRType System_RuntimeFieldHandle = null;
		public IRType System_RuntimeMethodHandle = null;
		public IRType System_RuntimeTypeHandle = null;
		public IRType System_SByte = null;
		public IRType System_Single = null;
		public IRType System_String = null;
		public IRType System_Type = null;
		public IRType System_TypedReference = null;
		public IRType System_UInt16 = null;
		public IRType System_UInt32 = null;
		public IRType System_UInt64 = null;
		public IRType System_UIntPtr = null;
		public IRType System_ValueType = null;
		public IRType System_Void = null;

		// AOT Only
		public IRMethod System_GC_AllocateArrayOfType = null;
		public IRMethod System_GC_AllocateObject = null;
		public IRMethod System_GC_AllocateStringFromUTF16 = null;
		public IRMethod System_GC_BoxObject = null;

		public IRAppDomain()
		{
			AddAssembly(new IRAssembly(this, new CLIFile("mscorlib", File.ReadAllBytes("mscorlib.dll")), true));
		}

		private void AddAssembly(IRAssembly pAssembly)
		{
			Assemblies.Add(pAssembly);
			AssemblyFileLookup.Add(pAssembly.File, pAssembly);
			AssemblyFileReferenceNameLookup.Add(pAssembly.File.ReferenceName, pAssembly);
		}

		public IRAssembly CreateAssembly(CLIFile pFile)
		{
			IRAssembly assembly = null;
			if (AssemblyFileReferenceNameLookup.TryGetValue(pFile.ReferenceName, out assembly)) return assembly;
			assembly = new IRAssembly(this, pFile, false);
			AddAssembly(assembly);
			foreach (AssemblyRefData assemblyRefData in pFile.AssemblyRefTable) CreateAssembly(new CLIFile(assemblyRefData.Name, File.ReadAllBytes(assemblyRefData.Name + ".dll")));
			return assembly;
		}

		internal void CacheCOR(IRAssembly pAssembly)
		{
			foreach (IRType type in pAssembly.Types)
			{
				if (type.Namespace == "System")
				{
					switch (type.Name)
					{
						case "Array": System_Array = type; break;
						case "Boolean": System_Boolean = type; break;
						case "Byte": System_Byte = type; break;
						case "Char": System_Char = type; break;
						case "Double": System_Double = type; break;
						case "Enum": System_Enum = type; break;
						case "Exception": System_Exception = type; break;
						case "GC": System_GC = type; break;
						case "Int16": System_Int16 = type; break;
						case "Int32": System_Int32 = type; break;
						case "Int64": System_Int64 = type; break;
						case "IntPtr": System_IntPtr = type; break;
						case "Object": System_Object = type; break;
						case "RuntimeArgumentHandle": System_RuntimeArgumentHandle = type; break;
						case "RuntimeFieldHandle": System_RuntimeFieldHandle = type; break;
						case "RuntimeMethodHandle": System_RuntimeMethodHandle = type; break;
						case "RuntimeTypeHandle": System_RuntimeTypeHandle = type; break;
						case "SByte": System_SByte = type; break;
						case "Single": System_Single = type; break;
						case "String": System_String = type; break;
						case "Type": System_Type = type; break;
						case "TypedReference": System_TypedReference = type; break;
						case "UInt16": System_UInt16 = type; break;
						case "UInt32": System_UInt32 = type; break;
						case "UInt64": System_UInt64 = type; break;
						case "UIntPtr": System_UIntPtr = type; break;
						case "ValueType": System_ValueType = type; break;
						case "Void": System_Void = type; break;
						default: break;
					}
				}
			}

			foreach (IRMethod method in pAssembly.Methods)
			{
				if (method.ParentType == System_GC)
				{
					switch (method.Name)
					{
						case "AllocateArrayOfType": System_GC_AllocateArrayOfType = method; break;
						case "AllocateObject": System_GC_AllocateObject = method; break;
						case "AllocateStringFromUTF16": System_GC_AllocateStringFromUTF16 = method; break;
						case "BoxObject": System_GC_BoxObject = method; break;
						default: break;
					}
				}
			}
		}

		[Conditional("ProfileMemory")]
		private static void PauseForProfiler()
		{
			Console.WriteLine("Press Enter to Continue...");
			Console.ReadLine();
		}

		[Conditional("ProfileSpeed")]
		private static void ProfileStart(Stopwatch sw)
		{
			PauseForProfiler();
			sw.Start();
		}

		[Conditional("ProfileSpeed")]
		private static void ProfileWrite(string msg, Stopwatch sw)
		{
			sw.Stop();
			Console.WriteLine(String.Format("{0} took {1}MS", msg, sw.ElapsedMilliseconds));
			sw.Reset();
		}

		private static void FullGCCollect()
		{
			GC.Collect();
			GC.WaitForPendingFinalizers();
			GC.Collect();
		}

		public int CurrentCompileStage { get; private set; }

		public IRAssembly LoadEntryAssembly(CLIFile pFile)
		{
			Stopwatch sw = new Stopwatch();
			ProfileStart(sw);
			IRAssembly assembly = CreateAssembly(pFile);
			ProfileWrite("Creating the main IRAssembly", sw);
			ProfileStart(sw);
			CurrentCompileStage = 1;
			Assemblies.ForEach(a => a.LoadStage1());
			ProfileWrite("Stage 1", sw);
			ProfileStart(sw);
			CurrentCompileStage = 2;
			Assemblies.ForEach(a => a.LoadStage2());
			ProfileWrite("Stage 2", sw);
			ProfileStart(sw);
			CurrentCompileStage = 3;
			Assemblies.ForEach(a => a.LoadStage3());
			ProfileWrite("Stage 3", sw);

			ProfileStart(sw);
			AssemblyFileLookup = null;
			AssemblyFileReferenceNameLookup = null;
			IRType.MVarPlaceholders = null;
			IRType.VarPlaceholders = null;
			FullGCCollect();
			ProfileWrite("Stage 3.5 GC", sw);

			ProfileStart(sw);
			CurrentCompileStage = 4;
			Assemblies.ForEach(a => a.LoadStage4());
			foreach (var t in ArrayTypes.Values)
			{
				if (t.Resolved) 
				{
					foreach (var m in t.Methods)
					{
						if (m.Resolved) { }
					}
				}
			}
			ProfileWrite("Stage 4", sw);

			ProfileStart(sw);
			Assemblies.ForEach(a =>
			{
				a.Fields = null;
				a.File = null;
				a.Methods = null;
				a.Types = null;
			});
			Assemblies = null;
			IRType.GenericTypes = null;
			UnmanagedPointerTypes = null;
			ArrayTypes = null;
			FullGCCollect();
			ProfileWrite("Stage 4.5 GC", sw);

			//Console.WriteLine("There are {0} methods from arrays, {1} from managed pointers, and {2} from unmanaged pointers", Methods.FindAll(m => m.ParentType.IsArrayType).Count, Methods.FindAll(m => m.ParentType.IsManagedPointerType).Count, Methods.FindAll(m => m.ParentType.IsUnmanagedPointerType).Count);
			ProfileStart(sw);
			CurrentCompileStage = 5;
			LoadStage5();
			ProfileWrite("Stage 5", sw);
			ProfileStart(sw);
			CurrentCompileStage = 6;
			LoadStage6(true);
			ProfileWrite("Stage 6", sw);

			ProfileStart(sw);
			// Massive help to memory usage.
			FullGCCollect();
			ProfileWrite("Stage 6.5 GC", sw);

			ProfileStart(sw);
			CurrentCompileStage = 7;
			LoadStage7();
			ProfileWrite("Stage 7", sw);

			ProfileStart(sw);
			CurrentCompileStage = 8;
			LoadStage8();
			ProfileWrite("Stage 8", sw);

			ProfileStart(sw);
			StaticFields = null;
			Methods = null;
			Types = null;
			ManagedPointerTypes = null;
			IRLinearizedLocation.KnownStaticConstructors = null;
			ClearCORTypes();
			FullGCCollect();
			ProfileWrite("Stage 8.5 GC", sw);

			ProfileStart(sw);
			CurrentCompileStage = 9;
			LoadStage9();
			ProfileWrite("Stage 9", sw);
			//Console.WriteLine("Dumping IRAppDomain...");
			//Dump();
			return assembly;
		}

		private void ClearCORTypes()
		{
			System_Array = null;
			System_Boolean = null;
			System_Byte = null;
			System_Char = null;
			System_Double = null;
			System_Enum = null;
			System_Exception = null;
			System_GC = null;
			System_GC_AllocateArrayOfType = null;
			System_GC_AllocateObject = null;
			System_GC_AllocateStringFromUTF16 = null;
			System_GC_BoxObject = null;
			System_Int16 = null;
			System_Int32 = null;
			System_Int64 = null;
			System_IntPtr = null;
			System_Object = null;
			System_RuntimeArgumentHandle = null;
			System_RuntimeFieldHandle = null;
			System_RuntimeMethodHandle = null;
			System_RuntimeTypeHandle = null;
			System_SByte = null;
			System_Single = null;
			System_String = null;
			System_Type = null;
			System_TypedReference = null;
			System_UInt16 = null;
			System_UInt32 = null;
			System_UInt64 = null;
			System_UIntPtr = null;
			System_ValueType = null;
			System_Void = null;
		}

		private static readonly List<IRTransformationPass> KnownTransformationPasses = new List<IRTransformationPass>()
		{
			// Type Transforms
			new IRGenericToStringImplementorTransformationPass(),

			// Method Transforms
			new IRInternalCallImplementationTransformationPass(),
			new IRCallDevirtualizationTransformationPass(),
			new IRStaticFieldLiteralInliningTransformationPass(),
			new IRSizeOfToConstantTransformationPass(),
			new IRInitalizeObjectSpecializationTransformationPass(),
			new IRInstructionSpecializationTransformationPass(),
		};

		private void LoadStage5()
		{
			Console.WriteLine("========== {0,-22} Stage 5 {0,-23} ==========", " ");
			Layout();

			KnownTransformationPasses.FindAll(tp => (tp.Type & IRTransformationPass.TransformType.Type) == IRTransformationPass.TransformType.Type).ForEach(tp =>
			{
				Types.ForEach(t => tp.Transform(t));
			});

			KnownTransformationPasses.FindAll(tp => (tp.Type & IRTransformationPass.TransformType.Method) == IRTransformationPass.TransformType.Method).ForEach(tp =>
			{
				Methods.ForEach(m => tp.Transform(m));
			});
		}

		private static readonly IROptimizationPass[][] KnownOptimizationPasses = new IROptimizationPass[][]
		{
			// First
			new IROptimizationPass[]
			{

			},
			// BeforeSSA
			new IROptimizationPass[]
			{
				new IRBranchRemovalOptimizationPass(),
			},
			// Enter SSA
			new IROptimizationPass[]
			{

			},
			// During SSA
			new IROptimizationPass[]
			{
				new IRMoveCompactingOptimizationPass(),
				new IRIndirectionRemovalOptimizationPass(),
			},
			// Leave SSA
			new IROptimizationPass[]
			{

			},
			// After SSA
			new IROptimizationPass[]
			{

			},
			// Last
			new IROptimizationPass[]
			{
				new IRNopKillingOptimizationPass(),
			},
		};

		public static void RemoveDeadCode(IRMethod pMethod)
		{
			IRControlFlowGraph cfg = IRControlFlowGraph.Build(pMethod);
			if (cfg == null) return;

			bool[] procs = new bool[pMethod.Instructions.Count];
			cfg.Nodes.ForEach(n => n.Instructions.ForEach(i => procs[i.IRIndex] = true));
			if (!procs.TrueForAll(f => f))
			{
				pMethod.Instructions.ImmediateRetargetModifiedInstructions = false;
				for (int i = 0; i < pMethod.Instructions.Count; i++)
				{
					if (!procs[i])
						pMethod.Instructions[i] = new Instructions.IRNopInstruction();
				}
				pMethod.Instructions.FixModifiedTargetInstructions();
				pMethod.Instructions.ImmediateRetargetModifiedInstructions = true;
			}
		}

		private void LoadStage6(bool runOptimizations)
		{
			Console.WriteLine("========== {0,-22} Stage 6 {0,-23} ==========", " ");
			if (runOptimizations)
			{
				foreach (var op in KnownOptimizationPasses[(int)IROptimizationPass.RunLocation.First])
				{
					foreach (var m in Methods) 
						op.Run(m);
				}
				foreach (var op in KnownOptimizationPasses[(int)IROptimizationPass.RunLocation.BeforeSSA])
				{
					foreach (var m in Methods)
						op.Run(m);
				}

				foreach (var m in Methods)
					m.EnterSSA();

				foreach (var op in KnownOptimizationPasses[(int)IROptimizationPass.RunLocation.DuringSSA])
				{
					foreach (var m in Methods)
						op.Run(m);
				}

				foreach (var m in Methods)
					m.LeaveSSA();

				foreach (var op in KnownOptimizationPasses[(int)IROptimizationPass.RunLocation.AfterSSA])
				{
					foreach (var m in Methods)
						op.Run(m);
				}
				
				// Nothing in the "Last" RunLocation should produce dead code, it's only for the cleanup code.
				foreach (var m in Methods)
					RemoveDeadCode(m);

				foreach (var op in KnownOptimizationPasses[(int)IROptimizationPass.RunLocation.Last])
				{
					foreach (var m in Methods)
						op.Run(m);
				}
			}
			else
			{
				Console.WriteLine("Not Performing Optimizations...");
			}
		}

		private LIR.LIRCompileUnit mainUnit;
		private void LoadStage7()
		{
			Console.WriteLine("========== {0,-22} Stage 7 {0,-23} ==========", " ");
			mainUnit = new LIR.LIRCompileUnit();
			Methods.ForEach(m => m.CreateLIRMethod());
			Methods.ForEach(m => mainUnit.AddMethod(m.LIRMethod));
			Methods.ForEach(m => m.PopulateLIRMethodBody());
		}
		
		private void LoadStage8()
		{
			Console.WriteLine("========== {0,-22} Stage 8 {0,-23} ==========", " ");
			foreach (var t in Types)
			{
				t.AddToCompileUnit(mainUnit);
				foreach (var f in t.Fields)
				{
					f.AddToCompileUnit(mainUnit);
				}
			}
			foreach (var m in Methods)
			{
				m.AddToCompileUnit(mainUnit);
			}
			StaticFields.ForEach(f => f.AddToCompileUnit(mainUnit));
		}

		private void LoadStage9()
		{
			Console.WriteLine("========== {0,-22} Stage 9 {0,-23} ==========", " ");
			MemoryStream strm = new MemoryStream();
			mainUnit.Compile(strm);
		}

		private void Layout()
		{
			Types.ForEach(t => t.LayoutFields());
			LayoutStaticFields();
			Methods.ForEach(m => m.LayoutParameters());
			Methods.ForEach(m => m.LayoutLocals());
			Types.ForEach(t => t.CreateVirtualMethodTree());
			Types.ForEach(t => t.CreateInterfaceImplementationMap());
			LayoutInterfaceTable();
		}

		private static Dictionary<IRType, List<IRType>> KnownInterfaces = new Dictionary<IRType, List<IRType>>();
		private static int GetAvailableSlot(IRType interfaceType, int startIndex)
		{
			List<IRType> tpTable;
			if (!KnownInterfaces.TryGetValue(interfaceType, out tpTable))
				throw new Exception("Unknown interface type!");
			if (tpTable.Count < startIndex)
				return startIndex + 1;
			else
			{
				for (int i = startIndex; i < tpTable.Count; i++)
				{
					if (tpTable[i] == null)
						return i;
				}
				return tpTable.Count;
			}
		}

		private static void AssignToInterfaceMap(IRType tp, int idx, IEnumerable<IRType> interfaces)
		{
			foreach (var iFace in interfaces)
			{
				List<IRType> tMap;
				if (!KnownInterfaces.TryGetValue(iFace, out tMap))
					throw new Exception("Unknown interface!");
				while (idx >= tMap.Count)
					tMap.Add(null);
				if (tMap[idx] != null)
					throw new Exception("We shouldn't be re-assigning to an index!");
				tMap[idx] = tp;
			}
			tp.InterfaceTableTypeID = idx;
		}

		private static void AddToInterfaceTable(IRType tp)
		{
			if (tp.InterfaceImplementationMap.Count > 0)
			{
				int tryIndex = 0;
				while (true)
				{
					List<int> lIdxs = new List<int>(tp.InterfaceImplementationMap.Count);
					foreach (var iFace in tp.InterfaceImplementationMap.Keys)
					{
						lIdxs.Add(GetAvailableSlot(iFace, tryIndex));
					}
					bool same = true;
					int lst = lIdxs[0];
					int max = lIdxs[0];
					for (int i = 1; i < lIdxs.Count; i++)
					{
						if (lIdxs[i] != lst)
							same = false;
						if (lIdxs[i] > max)
							max = lIdxs[i];
					}
					if (same)
					{
						AssignToInterfaceMap(tp, lst, tp.InterfaceImplementationMap.Keys);
						break;
					}
					tryIndex = max;
				}
			}
		}

		private void LayoutInterfaceTable()
		{
			int i = 0;
			foreach (var v in Types.FindAll(t => t.IsInterface))
			{
				v.InterfaceID = i++;
				KnownInterfaces[v] = new List<IRType>();
			}
			int typesInTable = 0;
			foreach (var v in Types)
			{
				if (!v.IsInterface)
				{
					AddToInterfaceTable(v);
					typesInTable++;
				}
			}
			int maxTpIdx = 0;
			int total = 0;
			foreach (var v in KnownInterfaces.Values)
			{
				if (v.Count > maxTpIdx)
					maxTpIdx = v.Count;
				total += v.Count;
			}
			Console.WriteLine(String.Format("Fit {0} types implementing {1} interfaces in {2} total entries, with a maximum of {3} entries in one table", typesInTable, i, total, maxTpIdx));
		}

		private void LayoutStaticFields()
		{
			Types.ForEach(t => StaticFields.AddRange(t.Fields.FindAll(f => f.IsStatic && !f.IsLiteral)));
			int offset = 0;
			for (int i = 0; i < StaticFields.Count; i++)
			{
				StaticFields[i].Offset = offset;
				offset += StaticFields[i].Type.StackSize;
			}
		}

		// Dynamic Types
		private Dictionary<IRType, IRType> UnmanagedPointerTypes = new Dictionary<IRType, IRType>();
		private Dictionary<IRType, IRType> ManagedPointerTypes = new Dictionary<IRType, IRType>();
		private Dictionary<IRType, IRType> ArrayTypes = new Dictionary<IRType, IRType>();

		public IRType GetUnmanagedPointerType(IRType pValueAtPointerType)
		{
			IRType type = null;
			if (!UnmanagedPointerTypes.TryGetValue(pValueAtPointerType, out type))
			{
				type = System_IntPtr.Clone();
				type.UnmanagedPointerType = pValueAtPointerType;
				UnmanagedPointerTypes.Add(pValueAtPointerType, type);
			}
			return type;
		}

		public IRType GetManagedPointerType(IRType pValueAtPointerType)
		{
			IRType type = null;
			if (!ManagedPointerTypes.TryGetValue(pValueAtPointerType, out type))
			{
				type = System_IntPtr.Clone();
				type.ManagedPointerType = pValueAtPointerType;
				ManagedPointerTypes.Add(pValueAtPointerType, type);
			}
			return type;
		}

		public IRType GetArrayType(IRType pElementType)
		{
			IRType type = null;
			if (!ArrayTypes.TryGetValue(pElementType, out type))
			{
				type = System_Array.Clone();
				type.ArrayElementType = pElementType;
				ArrayTypes.Add(pElementType, type);
			}
			if (pElementType.IsTemporaryVar != type.ArrayElementType.IsTemporaryVar || pElementType.IsTemporaryMVar != type.ArrayElementType.IsTemporaryMVar)
			{
				throw new Exception();
			}
			return type;
		}

		public IRType PresolveGenericType(IRType pGenericType, List<IRType> pGenericParameterTypes)
		{
			if (pGenericType == null) throw new Exception();
			IRType type = new IRType(pGenericType.Assembly);
			type.Name = pGenericType.Name;
			type.Namespace = pGenericType.Namespace;
			type.PresolvedType = true;
			type.GenericType = pGenericType;
			type.GenericParameters.AddRange(pGenericParameterTypes);
			return type;
		}

		public IRMethod PresolveGenericMethod(IRMethod pGenericMethod, List<IRType> pGenericParameterTypes, List<IRType> pParentTypeGenericParameters = null)
		{
			IRMethod method = new IRMethod(pGenericMethod.Assembly);
			method.Name = pGenericMethod.Name;
			method.PresolvedMethod = true;
			method.GenericMethod = pGenericMethod;
			method.ReturnType = pGenericMethod.ReturnType;
			if (pParentTypeGenericParameters != null)
			{
				method.ParentType = PresolveGenericType(pGenericMethod.ParentType, pParentTypeGenericParameters);
			}
			else
			{
				method.ParentType = pGenericMethod.ParentType;
			}
			method.GenericParameters.AddRange(pGenericMethod.GenericParameters);
			method.GenericParameters.Merge(new IRGenericParameterList(pGenericParameterTypes));
			for (int index = 0; index < pGenericMethod.Parameters.Count; ++index)
			{
				IRParameter parameter = pGenericMethod.Parameters[index];
				IRParameter newParameter = new IRParameter(pGenericMethod.Assembly);
				newParameter.ParentMethod = method;
				newParameter.Type = parameter.Type;
				method.Parameters.Add(newParameter);
			}
			return method;
		}

		public bool CompareSignatures(SigType pSigTypeA, SigType pSigTypeB)
		{
			IRType typeA = PresolveType(pSigTypeA);
			IRType typeB = PresolveType(pSigTypeB);
			if (typeA.ArrayElementType != null)
			{
				if (typeB.ArrayElementType == null) return false;
				return typeA.ArrayElementType == typeB.ArrayElementType;
			}
			if (typeA.UnmanagedPointerType != null)
			{
				if (typeB.UnmanagedPointerType == null) return false;
				return typeA.UnmanagedPointerType == typeB.UnmanagedPointerType;
			}
			if (typeA.IsTemporaryVar || typeB.IsTemporaryVar)
			{
				if (!typeA.IsTemporaryVar || !typeB.IsTemporaryVar) return false;
				return typeA.TemporaryVarOrMVarIndex == typeB.TemporaryVarOrMVarIndex;
			}
			if (typeA.IsTemporaryMVar || typeB.IsTemporaryMVar)
			{
				if (!typeA.IsTemporaryMVar || !typeB.IsTemporaryMVar) return false;
				return typeA.TemporaryVarOrMVarIndex == typeB.TemporaryVarOrMVarIndex;
			}
			return typeA == typeB;
		}

		public bool CompareSignatures(MethodSig pMethodSigA, MethodSig pMethodSigB)
		{
			if (pMethodSigA.HasThis != pMethodSigB.HasThis) return false;
			if (pMethodSigA.ExplicitThis != pMethodSigB.ExplicitThis) return false;
			if (pMethodSigA.Default != pMethodSigB.Default) return false;
			if (pMethodSigA.VarArg != pMethodSigB.VarArg) return false;
			if (pMethodSigA.CCall != pMethodSigB.CCall) return false;
			if (pMethodSigA.STDCall != pMethodSigB.STDCall) return false;
			if (pMethodSigA.ThisCall != pMethodSigB.ThisCall) return false;
			if (pMethodSigA.FastCall != pMethodSigB.FastCall) return false;
			if (pMethodSigA.GenParamCount != pMethodSigB.GenParamCount) return false;
			if (pMethodSigA.Params.Count != pMethodSigB.Params.Count) return false;
			if (pMethodSigA.HasSentinel != pMethodSigB.HasSentinel) return false;
			if (pMethodSigA.SentinelIndex != pMethodSigB.SentinelIndex) return false;
			if (pMethodSigA.RetType.Void != pMethodSigB.RetType.Void) return false;
			if (!pMethodSigA.RetType.Void)
			{
				if (pMethodSigA.RetType.ByRef != pMethodSigB.RetType.ByRef) return false;
				if (pMethodSigA.RetType.TypedByRef != pMethodSigB.RetType.TypedByRef) return false;
				if (!pMethodSigA.RetType.TypedByRef && !CompareSignatures(pMethodSigA.RetType.Type, pMethodSigB.RetType.Type)) return false;
			}
			for (int index = 0; index < pMethodSigA.Params.Count; ++index)
			{
				if (pMethodSigA.Params[index].ByRef != pMethodSigB.Params[index].ByRef) return false;
				if (pMethodSigA.Params[index].TypedByRef != pMethodSigB.Params[index].TypedByRef) return false;
				if (!pMethodSigA.Params[index].TypedByRef && !CompareSignatures(pMethodSigA.Params[index].Type, pMethodSigB.Params[index].Type)) return false;
			}
			return true;
		}

		public IRType PresolveType(TypeDefData pTypeDefData)
		{
			if (pTypeDefData == null) throw new ArgumentNullException("pTypeDefData");
			return AssemblyFileLookup[pTypeDefData.CLIFile].Types[pTypeDefData.TableIndex];
		}

		public IRType PresolveType(TypeRefData pTypeRefData)
		{
			if (pTypeRefData.ExportedType != null)
			{
				switch (pTypeRefData.ExportedType.Implementation.Type)
				{
					case ImplementationIndex.ImplementationType.File:
						{
							IRAssembly assembly = null;
							if (!AssemblyFileReferenceNameLookup.TryGetValue(pTypeRefData.ExportedType.Implementation.File.Name, out assembly)) throw new KeyNotFoundException();
							IRType type = assembly.Types.Find(t => t.Namespace == pTypeRefData.TypeNamespace && t.Name == pTypeRefData.TypeName);
							if (type == null) throw new NullReferenceException();
							return type;
						}
					case ImplementationIndex.ImplementationType.AssemblyRef:
						{
							IRAssembly assembly = null;
							if (!AssemblyFileReferenceNameLookup.TryGetValue(pTypeRefData.ExportedType.Implementation.AssemblyRef.Name, out assembly)) throw new KeyNotFoundException();
							IRType type = assembly.Types.Find(t => t.Namespace == pTypeRefData.TypeNamespace && t.Name == pTypeRefData.TypeName);
							if (type == null) throw new NullReferenceException();
							return type;
						}
					default: break;
				}
				throw new NullReferenceException();
			}
			switch (pTypeRefData.ResolutionScope.Type)
			{
				case ResolutionScopeIndex.ResolutionScopeType.AssemblyRef:
					{
						IRAssembly assembly = null;
						if (!AssemblyFileReferenceNameLookup.TryGetValue(pTypeRefData.ResolutionScope.AssemblyRef.Name, out assembly)) throw new KeyNotFoundException();
						IRType type = assembly.Types.Find(t => t.Namespace == pTypeRefData.TypeNamespace && t.Name == pTypeRefData.TypeName);
						if (type == null) throw new NullReferenceException();
						return type;
					}
				case ResolutionScopeIndex.ResolutionScopeType.TypeRef:
					{
						IRType type = PresolveType(pTypeRefData.ResolutionScope.TypeRef);
						if (type == null) throw new NullReferenceException();
						IRType nestedType = type.NestedTypes.Find(t => t.Namespace == pTypeRefData.TypeNamespace && t.Name == pTypeRefData.TypeName);
						if (nestedType == null) throw new NullReferenceException();
						return nestedType;
					}
			}
			throw new NullReferenceException();
		}

		public IRType PresolveType(TypeSpecData pTypeSpecData) { return PresolveType(pTypeSpecData.ExpandedSignature); }

		public IRType PresolveType(TypeDefRefOrSpecIndex pTypeDefRefOrSpecIndex)
		{
			switch (pTypeDefRefOrSpecIndex.Type)
			{
				case TypeDefRefOrSpecIndex.TypeDefRefOrSpecType.TypeDef: return PresolveType(pTypeDefRefOrSpecIndex.TypeDef);
				case TypeDefRefOrSpecIndex.TypeDefRefOrSpecType.TypeRef: return PresolveType(pTypeDefRefOrSpecIndex.TypeRef);
				case TypeDefRefOrSpecIndex.TypeDefRefOrSpecType.TypeSpec: return PresolveType(pTypeDefRefOrSpecIndex.TypeSpec.ExpandedSignature);
				default: break;
			}
			throw new NullReferenceException();
		}

		public IRType PresolveType(MetadataToken pMetadataToken)
		{
			switch (pMetadataToken.Table)
			{
				case MetadataTables.TypeDef: return PresolveType((TypeDefData)pMetadataToken.Data);
				case MetadataTables.TypeRef: return PresolveType((TypeRefData)pMetadataToken.Data);
				case MetadataTables.TypeSpec: return PresolveType(((TypeSpecData)pMetadataToken.Data).ExpandedSignature);
				default: break;
			}
			throw new NullReferenceException();
		}

		public IRType PresolveType(SigType pSigType)
		{
			IRType type = null;
			switch (pSigType.ElementType)
			{
				case SigElementType.Void: type = System_Void; break;
				case SigElementType.Boolean: type = System_Boolean; break;
				case SigElementType.Char: type = System_Char; break;
				case SigElementType.I1: type = System_SByte; break;
				case SigElementType.U1: type = System_Byte; break;
				case SigElementType.I2: type = System_Int16; break;
				case SigElementType.U2: type = System_UInt16; break;
				case SigElementType.I4: type = System_Int32; break;
				case SigElementType.U4: type = System_UInt32; break;
				case SigElementType.I8: type = System_Int64; break;
				case SigElementType.U8: type = System_UInt64; break;
				case SigElementType.R4: type = System_Single; break;
				case SigElementType.R8: type = System_Double; break;
				case SigElementType.String: type = System_String; break;
				case SigElementType.Pointer:
					{
						if (pSigType.PtrVoid) type = GetUnmanagedPointerType(System_Void);
						else type = GetUnmanagedPointerType(PresolveType(pSigType.PtrType));
						break;
					}
				case SigElementType.ValueType: type = PresolveType(pSigType.CLIFile.ExpandTypeDefRefOrSpecToken(pSigType.ValueTypeDefOrRefOrSpecToken)); break;
				case SigElementType.Class: type = PresolveType(pSigType.CLIFile.ExpandTypeDefRefOrSpecToken(pSigType.ClassTypeDefOrRefOrSpecToken)); break;
				case SigElementType.Array: type = GetArrayType(PresolveType(pSigType.ArrayType)); break;
				case SigElementType.GenericInstantiation:
					{
						IRType genericType = PresolveType(pSigType.CLIFile.ExpandTypeDefRefOrSpecToken(pSigType.GenericInstTypeDefOrRefOrSpecToken));
						List<IRType> genericTypeParameters = new List<IRType>();
						foreach (SigType paramType in pSigType.GenericInstGenArgs) genericTypeParameters.Add(PresolveType(paramType));
						type = PresolveGenericType(genericType, genericTypeParameters);
						break;
					}
				case SigElementType.IPointer: type = System_IntPtr; break;
				case SigElementType.UPointer: type = System_UIntPtr; break;
				case SigElementType.Object: type = System_Object; break;
				case SigElementType.SingleDimensionArray: type = GetArrayType(PresolveType(pSigType.SZArrayType)); break;
				case SigElementType.Var: type = IRType.GetVarPlaceholder(pSigType.VarNumber); break;
				case SigElementType.MethodVar: type = IRType.GetMVarPlaceholder(pSigType.MVarNumber); break;
				case SigElementType.Type: type = System_Type; break;
				default: break;
			}
			if (type == null) throw new NullReferenceException();
			return type;
		}

		public IRType PresolveType(FieldSig pFieldSig) { return PresolveType(pFieldSig.Type); }

		public IRType PresolveType(SigRetType pSigRetType)
		{
			IRType type = null;
			if (!pSigRetType.Void)
			{
				if (pSigRetType.TypedByRef) type = System_TypedReference;
				else
				{
					type = PresolveType(pSigRetType.Type);
					if (pSigRetType.ByRef) type = GetManagedPointerType(type);
				}
			}
			return type;
		}

		public IRType PresolveType(SigParam pSigParam)
		{
			IRType type = null;
			if (pSigParam.TypedByRef) type = System_TypedReference;
			else
			{
				type = PresolveType(pSigParam.Type);
				if (pSigParam.ByRef) type = GetManagedPointerType(type);
			}
			return type;
		}

		public IRType PresolveType(SigLocalVar pSigLocalVar)
		{
			IRType type = null;
			if (pSigLocalVar.TypedByRef) type = System_TypedReference;
			else
			{
				type = PresolveType(pSigLocalVar.Type);
				if (pSigLocalVar.ByRef) type = GetManagedPointerType(type);
			}
			return type;
		}

		public IRMethod PresolveMethod(MethodDefData pMethodDefData)
		{
			if (pMethodDefData == null) throw new ArgumentNullException("pMethodDefData");
			return AssemblyFileLookup[pMethodDefData.CLIFile].Methods[pMethodDefData.TableIndex];
		}

		public IRMethod PresolveMethod(MemberRefData pMemberRefData)
		{
			if (!pMemberRefData.IsMethodRef) throw new ArgumentException();
			switch (pMemberRefData.Class.Type)
			{
				case MemberRefParentIndex.MemberRefParentType.TypeDef:
					{
						IRType type = PresolveType(pMemberRefData.Class.TypeDef);
						foreach (IRMethod method in type.Methods)
						{
							if (method.CompareSignature(pMemberRefData)) return method;
						}
						throw new NullReferenceException();
					}
				case MemberRefParentIndex.MemberRefParentType.TypeRef:
					{
						IRType type = PresolveType(pMemberRefData.Class.TypeRef);
						foreach (IRMethod method in type.Methods)
						{
							if (method.CompareSignature(pMemberRefData)) return method;
						}
						throw new NullReferenceException();
					}
				case MemberRefParentIndex.MemberRefParentType.TypeSpec:
					{
						IRType type = PresolveType(pMemberRefData.Class.TypeSpec);
						foreach (IRMethod method in type.GenericType.Methods)
						{
							if (method.CompareSignature(pMemberRefData)) 
								return PresolveGenericMethod(method, new List<IRType>(), type.GenericParameters.ToList());
						}
						throw new NullReferenceException();
					}
			}
			throw new NullReferenceException();
		}

		public IRMethod PresolveMethod(MethodSpecData pMethodSpecData)
		{
			IRMethod genericMethod = PresolveMethod(pMethodSpecData.Method);
			List<IRType> genericMethodParameters = new List<IRType>();
			foreach (SigType paramType in pMethodSpecData.ExpandedInstantiation.GenArgs) 
				genericMethodParameters.Add(PresolveType(paramType));
			return PresolveGenericMethod(genericMethod, genericMethodParameters);
		}

		public IRMethod PresolveMethod(MethodDefOrRefIndex pMethodDefOrRefIndex)
		{
			switch (pMethodDefOrRefIndex.Type)
			{
				case MethodDefOrRefIndex.MethodDefOrRefType.MethodDef: return PresolveMethod(pMethodDefOrRefIndex.MethodDef);
				case MethodDefOrRefIndex.MethodDefOrRefType.MemberRef: return PresolveMethod(pMethodDefOrRefIndex.MemberRef);
			}
			throw new NullReferenceException();
		}

		public IRMethod PresolveMethod(MetadataToken pMetadataToken)
		{
			switch (pMetadataToken.Table)
			{
				case MetadataTables.MethodDef: return PresolveMethod((MethodDefData)pMetadataToken.Data);
				case MetadataTables.MemberRef: return PresolveMethod((MemberRefData)pMetadataToken.Data);
				case MetadataTables.MethodSpec: return PresolveMethod((MethodSpecData)pMetadataToken.Data);
				default: break;
			}
			throw new NullReferenceException();
		}

		public IRField PresolveField(FieldData pFieldData)
		{
			if (pFieldData == null) throw new ArgumentNullException("pFieldData");
			return AssemblyFileLookup[pFieldData.CLIFile].Fields[pFieldData.TableIndex];
		}

		public IRField PresolveField(MemberRefData pMemberRefData)
		{
			if (!pMemberRefData.IsFieldRef) throw new ArgumentException();
			switch (pMemberRefData.Class.Type)
			{
				case MemberRefParentIndex.MemberRefParentType.TypeDef:
					{
						IRType type = PresolveType(pMemberRefData.Class.TypeDef);
						foreach (IRField field in type.Fields)
						{
							if (field.CompareSignature(pMemberRefData)) return field;
						}
						throw new NullReferenceException();
					}
				case MemberRefParentIndex.MemberRefParentType.TypeRef:
					{
						IRType type = PresolveType(pMemberRefData.Class.TypeRef);
						foreach (IRField field in type.Fields)
						{
							if (field.CompareSignature(pMemberRefData)) return field;
						}
						throw new NullReferenceException();
					}
				case MemberRefParentIndex.MemberRefParentType.TypeSpec:
					{
						IRType type = PresolveType(pMemberRefData.Class.TypeSpec);
						foreach (IRField field in type.GenericType.Fields)
						{
							if (field.CompareSignature(pMemberRefData)) return field;
						}
						throw new NullReferenceException();
					}
			}
			throw new NullReferenceException();
		}

		public IRField PresolveField(MetadataToken pMetadataToken)
		{
			switch (pMetadataToken.Table)
			{
				case MetadataTables.Field: return PresolveField((FieldData)pMetadataToken.Data);
				case MetadataTables.MemberRef: return PresolveField((MemberRefData)pMetadataToken.Data);
				default: break;
			}
			throw new NullReferenceException();
		}


		public IRType BinaryNumericResult(IRType pValue1Type, IRType pValue2Type)
		{
			IRType resultType = null;
			if (pValue1Type == System_SByte ||
				pValue1Type == System_Byte ||
				pValue1Type == System_Int16 ||
				pValue1Type == System_UInt16 ||
				pValue1Type == System_Char ||
				pValue1Type == System_Int32 ||
				pValue1Type == System_UInt32)
			{
				if (pValue2Type == System_SByte ||
					pValue2Type == System_Byte ||
					pValue2Type == System_Int16 ||
					pValue2Type == System_UInt16 ||
					pValue2Type == System_Char ||
					pValue2Type == System_Int32 ||
					pValue2Type == System_UInt32 ||
					pValue2Type.IsEnumType)
				{
					resultType = System_Int32;
				}
				else if (pValue2Type == System_IntPtr ||
						 pValue2Type == System_UIntPtr)
				{
					resultType = System_IntPtr;
				}
				else throw new ArgumentException();
			}
			else if (pValue1Type == System_Boolean)
			{
				if (pValue2Type == System_Boolean)
				{
					resultType = System_Boolean;
				}
				else throw new ArgumentException();
			}
			else if (pValue1Type == System_Int64 ||
					 pValue1Type == System_UInt64)
			{
				if (pValue2Type == System_Int64 ||
					pValue2Type == System_UInt64)
				{
					resultType = System_Int64;
				}
				else throw new ArgumentException();
			}
			else if (pValue1Type == System_IntPtr ||
					 pValue1Type == System_UIntPtr)
			{
				if (pValue2Type == System_SByte ||
					pValue2Type == System_Byte ||
					pValue2Type == System_Int16 ||
					pValue2Type == System_UInt16 ||
					pValue2Type == System_Char ||
					pValue2Type == System_Int32 ||
					pValue2Type == System_UInt32 ||
					pValue2Type == System_IntPtr ||
					pValue2Type == System_UIntPtr)
				{
					resultType = System_IntPtr;
				}
				else throw new ArgumentException();
			}
			else if (pValue1Type.IsUnmanagedPointerType)
			{
				if (pValue2Type == System_SByte ||
					pValue2Type == System_Byte ||
					pValue2Type == System_Int16 ||
					pValue2Type == System_UInt16 ||
					pValue2Type == System_Char ||
					pValue2Type == System_Int32 ||
					pValue2Type == System_UInt32 ||
					pValue2Type == System_IntPtr ||
					pValue2Type == System_UIntPtr ||
					pValue2Type.IsUnmanagedPointerType)
				{
					resultType = System_IntPtr;
				}
				else throw new ArgumentException();
			}
			else if (pValue1Type == System_Single ||
					 pValue1Type == System_Double)
			{
				if (pValue2Type == System_Single ||
					pValue2Type == System_Double)
				{
					resultType = System_Double;
				}
				else throw new ArgumentException();
			}
			else if (pValue1Type.IsEnumType &&
					 pValue2Type.IsEnumType &&
					 pValue1Type == pValue2Type)
			{
				resultType = pValue1Type;
			}
			else if (pValue1Type.IsEnumType &&
					 !pValue2Type.IsEnumType)
			{
				if (pValue2Type == System_SByte ||
					pValue2Type == System_Byte ||
					pValue2Type == System_Int16 ||
					pValue2Type == System_UInt16 ||
					pValue2Type == System_Char ||
					pValue2Type == System_Int32 ||
					pValue2Type == System_UInt32)
				{
					resultType = pValue1Type;
				}
				else throw new ArgumentException();
			}
			else throw new ArgumentException();
			return resultType;
		}

		public IRType ShiftNumericResult(IRType pValueType, IRType pShiftAmountType)
		{
			IRType resultType = null;
			if (pValueType == System_SByte ||
				pValueType == System_Byte ||
				pValueType == System_Int16 ||
				pValueType == System_UInt16 ||
				pValueType == System_Char ||
				pValueType == System_Int32 ||
				pValueType == System_UInt32)
			{
				if (pShiftAmountType == System_SByte ||
					pShiftAmountType == System_Byte ||
					pShiftAmountType == System_Int16 ||
					pShiftAmountType == System_UInt16 ||
					pShiftAmountType == System_Char ||
					pShiftAmountType == System_Int32 ||
					pShiftAmountType == System_UInt32 ||
					pShiftAmountType == System_IntPtr ||
					pShiftAmountType == System_UIntPtr)
				{
					resultType = System_Int32;
				}
				else throw new ArgumentException();
			}
			else if (pValueType == System_Int64 ||
					 pValueType == System_UInt64)
			{
				if (pShiftAmountType == System_Int32 ||
					pShiftAmountType == System_Int64 ||
					pShiftAmountType == System_UInt64)
				{
					resultType = System_Int64;
				}
				else throw new ArgumentException();
			}
			else if (pValueType == System_IntPtr ||
					 pValueType == System_UIntPtr)
			{
				if (pShiftAmountType == System_SByte ||
					pShiftAmountType == System_Byte ||
					pShiftAmountType == System_Int16 ||
					pShiftAmountType == System_UInt16 ||
					pShiftAmountType == System_Char ||
					pShiftAmountType == System_Int32 ||
					pShiftAmountType == System_UInt32 ||
					pShiftAmountType == System_IntPtr ||
					pShiftAmountType == System_UIntPtr)
				{
					resultType = System_IntPtr;
				}
				else throw new ArgumentException();
			}
			else if (pValueType.IsEnumType)
			{
				return ShiftNumericResult(pValueType.UnderlyingEnumType, pShiftAmountType);
			}
			else throw new ArgumentException();
			return resultType;
		}

		public void Dump()
		{
			IndentableStreamWriter writer = new IndentableStreamWriter("AppDomainDump.txt");
			writer.WriteLine("IRAppDomain 1");
			writer.WriteLine("{");
			writer.Indent++;
			Assemblies.ForEach(a => a.Dump(writer));
			writer.WriteLine("ResolvedTypes {0}", Types.Count);
			writer.WriteLine("{");
			writer.Indent++;
			Types.ForEach(t => t.Dump(writer));
			writer.Indent--;
			writer.WriteLine("}");
			writer.WriteLine("ResolvedMethods {0}", Methods.Count);
			writer.WriteLine("{");
			writer.Indent++;
			Methods.ForEach(m => m.Dump(writer));
			writer.Indent--;
			writer.WriteLine("}");
			//writer.WriteLine("GenericTypes {0}", IRType.GenericTypes.Count);
			//writer.WriteLine("{");
			//writer.Indent++;
			//IRType.GenericTypes.ForEach(t => t.Value.Dump(writer));
			//writer.Indent--;
			//writer.WriteLine("}");
			writer.Indent--;
			writer.WriteLine("}");
			writer.Close();
		}
	}
}
