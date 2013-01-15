using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Proton.VM.IR
{
	[StructLayout(LayoutKind.Explicit, Pack=1)]
	public sealed class IRLinearizedLocation
	{
		public struct LocalLocationData { public int LocalIndex; }
		[FieldOffset(0)]
		public LocalLocationData Local;

		public struct LocalAddressLocationData { public int LocalIndex; }
		[FieldOffset(0)]
		public LocalAddressLocationData LocalAddress;

		public struct ParameterLocationData { public int ParameterIndex; }
		[FieldOffset(0)]
		public ParameterLocationData Parameter;

		public struct ParameterAddressLocationData { public int ParameterIndex; }
		[FieldOffset(0)]
		public ParameterAddressLocationData ParameterAddress;

		public struct ConstantI4LocationData { public int Value; }
		[FieldOffset(0)]
		public ConstantI4LocationData ConstantI4;

		public struct ConstantI8LocationData { public long Value; }
		[FieldOffset(0)]
		public ConstantI8LocationData ConstantI8;

		public struct ConstantR4LocationData { public float Value; }
		[FieldOffset(0)]
		public ConstantR4LocationData ConstantR4;

		public struct ConstantR8LocationData { public double Value; }
		[FieldOffset(0)]
		public ConstantR8LocationData ConstantR8;
		// 49 unmanaged
		// 144 managed (x86)
		// 288 managed (x64)

		[StructLayout(LayoutKind.Sequential, Pack = 8, Size = 32)]
		public struct FieldLocationData
		{
			public IRField Field;
			public IRLinearizedLocation FieldLocation;
			private object padA;
			private object padB;
		}
		[FieldOffset(8)]
		public FieldLocationData Field;

		[StructLayout(LayoutKind.Sequential, Pack = 8, Size = 32)]
		public struct FieldAddressLocationData
		{
			public IRField Field;
			public IRLinearizedLocation FieldLocation;
			private object padA;
			private object padB;
		}
		[FieldOffset(8)]
		public FieldAddressLocationData FieldAddress;

		[StructLayout(LayoutKind.Sequential, Pack = 8, Size = 32)]
		public struct StaticFieldLocationData 
		{
			public IRField Field;
			private object padA;
			private object padB;
			private object padC;
		}
		[FieldOffset(8)]
		public StaticFieldLocationData StaticField;

		[StructLayout(LayoutKind.Sequential, Pack = 8, Size = 32)]
		public struct StaticFieldAddressLocationData 
		{
			public IRField Field;
			private object padA;
			private object padB;
			private object padC;
		}
		[FieldOffset(8)]
		public StaticFieldAddressLocationData StaticFieldAddress;

		[StructLayout(LayoutKind.Sequential, Pack = 8, Size = 32)]
		public struct IndirectLocationData
		{
			public IRType Type;
			public IRLinearizedLocation AddressLocation;
			private object padA;
			private object padB;
		}
		[FieldOffset(8)]
		public IndirectLocationData Indirect;

		[StructLayout(LayoutKind.Sequential, Pack = 8, Size = 32)]
		public struct SizeOfLocationData 
		{
			public IRType Type;
			private object padA;
			private object padB;
			private object padC;
		}
		[FieldOffset(8)]
		public SizeOfLocationData SizeOf;

		[StructLayout(LayoutKind.Sequential, Pack = 8, Size = 32)]
		public struct ArrayElementLocationData
		{
			public IRType ElementType;
			public IRLinearizedLocation ArrayLocation;
			public IRLinearizedLocation IndexLocation;
			private object padA;
		}
		[FieldOffset(8)]
		public ArrayElementLocationData ArrayElement;

		[StructLayout(LayoutKind.Sequential, Pack = 8, Size = 32)]
		public struct ArrayElementAddressLocationData
		{
			public IRType ElementType;
			public IRLinearizedLocation ArrayLocation;
			public IRLinearizedLocation IndexLocation;
			private object padA;
		}
		[FieldOffset(8)]
		public ArrayElementAddressLocationData ArrayElementAddress;

		[StructLayout(LayoutKind.Sequential, Pack = 8, Size = 32)]
		public struct ArrayLengthLocationData 
		{
			public IRLinearizedLocation ArrayLocation;
			private object padA;
			private object padB;
			private object padC;
		}
		[FieldOffset(8)]
		public ArrayLengthLocationData ArrayLength;

		[StructLayout(LayoutKind.Sequential, Pack = 8, Size = 32)]
		public struct FunctionAddressLocationData
		{
			private object mVirtual;
			public IRLinearizedLocation InstanceLocation;
			public IRMethod Method;
			private object padA;

			public bool Virtual { get { return (bool)(mVirtual ?? (mVirtual = false)); } set { mVirtual = value; } }
		}
		[FieldOffset(8)]
		public FunctionAddressLocationData FunctionAddress;

		[StructLayout(LayoutKind.Sequential, Pack = 8, Size = 32)]
		public struct RuntimeHandleLocationData
		{
			public IRType HandleType;
			public IRType TargetType;
			public IRMethod TargetMethod;
			public IRField TargetField;
		}
		[FieldOffset(8)]
		public RuntimeHandleLocationData RuntimeHandle;

		[StructLayout(LayoutKind.Sequential, Pack = 8, Size = 32)]
		public struct StringLocationData 
		{
			public string Value;
			private object padA;
			private object padB;
			private object padC;
		}
		[FieldOffset(8)]
		public StringLocationData String;

		[StructLayout(LayoutKind.Sequential, Pack = 8, Size = 32)]
		public struct PhiLocationData
		{
			public List<IRLinearizedLocation> SourceLocations;
			private object padA;
			private object padB;
			private object padC;
		}
		[FieldOffset(8)]
		public PhiLocationData Phi;

		[FieldOffset(40)]
		public IRInstruction ParentInstruction;
		[FieldOffset(48)]
		public IRLinearizedLocationType Type;

		private static int sTempID = 0;
		[FieldOffset(52)]
		internal int mTempID = 0;
		public IRLinearizedLocation(IRInstruction pParentInstruction, IRLinearizedLocationType pType)
		{
			mTempID = sTempID++;
			ParentInstruction = pParentInstruction;
			Type = pType;
		}
		public IRLinearizedLocation(IRInstruction pParentInstruction, IRLinearizedLocation pLinearizedTarget)
		{
			mTempID = sTempID++;
			ParentInstruction = pParentInstruction;
			Type = pLinearizedTarget.Type;
			switch (Type)
			{
				case IRLinearizedLocationType.Null: break;
				case IRLinearizedLocationType.Local: Local = pLinearizedTarget.Local; break;
				case IRLinearizedLocationType.LocalAddress: LocalAddress = pLinearizedTarget.LocalAddress; break;
				case IRLinearizedLocationType.Parameter: Parameter = pLinearizedTarget.Parameter; break;
				case IRLinearizedLocationType.ParameterAddress: ParameterAddress = pLinearizedTarget.ParameterAddress; break;
				case IRLinearizedLocationType.ConstantI4: ConstantI4 = pLinearizedTarget.ConstantI4; break;
				case IRLinearizedLocationType.ConstantI8: ConstantI8 = pLinearizedTarget.ConstantI8; break;
				case IRLinearizedLocationType.ConstantR4: ConstantR4 = pLinearizedTarget.ConstantR4; break;
				case IRLinearizedLocationType.ConstantR8: ConstantR8 = pLinearizedTarget.ConstantR8; break;
				case IRLinearizedLocationType.Field:
					Field = pLinearizedTarget.Field;
					Field.FieldLocation = pLinearizedTarget.Field.FieldLocation.Clone(pParentInstruction);
					break;
				case IRLinearizedLocationType.FieldAddress:
					FieldAddress = pLinearizedTarget.FieldAddress;
					FieldAddress.FieldLocation = pLinearizedTarget.FieldAddress.FieldLocation.Clone(pParentInstruction);
					break;
				case IRLinearizedLocationType.StaticField: StaticField = pLinearizedTarget.StaticField; break;
				case IRLinearizedLocationType.StaticFieldAddress: StaticFieldAddress = pLinearizedTarget.StaticFieldAddress; break;
				case IRLinearizedLocationType.Indirect:
					Indirect = pLinearizedTarget.Indirect;
					Indirect.AddressLocation = pLinearizedTarget.Indirect.AddressLocation.Clone(pParentInstruction);
					break;
				case IRLinearizedLocationType.SizeOf: SizeOf = pLinearizedTarget.SizeOf; break;
				case IRLinearizedLocationType.ArrayElement:
					ArrayElement = pLinearizedTarget.ArrayElement;
					ArrayElement.ArrayLocation = pLinearizedTarget.ArrayElement.ArrayLocation.Clone(pParentInstruction);
					ArrayElement.IndexLocation = pLinearizedTarget.ArrayElement.IndexLocation.Clone(pParentInstruction);
					break;
				case IRLinearizedLocationType.ArrayElementAddress:
					ArrayElementAddress = pLinearizedTarget.ArrayElementAddress;
					ArrayElementAddress.ArrayLocation = pLinearizedTarget.ArrayElementAddress.ArrayLocation.Clone(pParentInstruction);
					ArrayElementAddress.IndexLocation = pLinearizedTarget.ArrayElementAddress.IndexLocation.Clone(pParentInstruction);
					break;
				case IRLinearizedLocationType.ArrayLength:
					ArrayLength = pLinearizedTarget.ArrayLength;
					ArrayLength.ArrayLocation = pLinearizedTarget.ArrayLength.ArrayLocation.Clone(pParentInstruction);
					break;
				case IRLinearizedLocationType.FunctionAddress:
					FunctionAddress = pLinearizedTarget.FunctionAddress;
					if (FunctionAddress.InstanceLocation != null) FunctionAddress.InstanceLocation = pLinearizedTarget.FunctionAddress.InstanceLocation.Clone(pParentInstruction);
					break;
				case IRLinearizedLocationType.RuntimeHandle: RuntimeHandle = pLinearizedTarget.RuntimeHandle; break;
				case IRLinearizedLocationType.String: String = pLinearizedTarget.String; break;
				case IRLinearizedLocationType.Phi:
					Phi = pLinearizedTarget.Phi;
					Phi.SourceLocations = new List<IRLinearizedLocation>(pLinearizedTarget.Phi.SourceLocations.Count);
					pLinearizedTarget.Phi.SourceLocations.ForEach(l => Phi.SourceLocations.Add(l.Clone(pParentInstruction)));
					break;
				default: throw new ArgumentException("Type");
			}
		}

		public IRLinearizedLocation Clone(IRInstruction pNewInstruction) { return new IRLinearizedLocation(pNewInstruction, this); }

		public void Resolve()
		{
			switch (Type)
			{
				case IRLinearizedLocationType.Null:
				case IRLinearizedLocationType.Local:
				case IRLinearizedLocationType.LocalAddress:
				case IRLinearizedLocationType.Parameter:
				case IRLinearizedLocationType.ParameterAddress:
				case IRLinearizedLocationType.ConstantI4:
				case IRLinearizedLocationType.ConstantI8:
				case IRLinearizedLocationType.ConstantR4:
				case IRLinearizedLocationType.ConstantR8:
				case IRLinearizedLocationType.String:
					break;

				case IRLinearizedLocationType.Field:
					{
						Field.Field.Resolve(ref Field.Field, ParentInstruction.ParentMethod.ParentType.GenericParameters, ParentInstruction.ParentMethod.GenericParameters);
						Field.FieldLocation.Resolve();
						break;
					}
				case IRLinearizedLocationType.FieldAddress:
					{
						FieldAddress.Field.Resolve(ref FieldAddress.Field, ParentInstruction.ParentMethod.ParentType.GenericParameters, ParentInstruction.ParentMethod.GenericParameters);
						FieldAddress.FieldLocation.Resolve();
						break;
					}
				case IRLinearizedLocationType.StaticField:
					{
						StaticField.Field.Resolve(ref StaticField.Field, ParentInstruction.ParentMethod.ParentType.GenericParameters, ParentInstruction.ParentMethod.GenericParameters);
						break;
					}
				case IRLinearizedLocationType.StaticFieldAddress:
					{
						StaticFieldAddress.Field.Resolve(ref StaticFieldAddress.Field, ParentInstruction.ParentMethod.ParentType.GenericParameters, ParentInstruction.ParentMethod.GenericParameters);
						break;
					}
				case IRLinearizedLocationType.Indirect:
					{
						Indirect.Type.Resolve(ref Indirect.Type, ParentInstruction.ParentMethod.ParentType.GenericParameters, ParentInstruction.ParentMethod.GenericParameters);
						Indirect.AddressLocation.Resolve();
						break;
					}
				case IRLinearizedLocationType.SizeOf:
					{
						SizeOf.Type.Resolve(ref SizeOf.Type, ParentInstruction.ParentMethod.ParentType.GenericParameters, ParentInstruction.ParentMethod.GenericParameters);
						break;
					}
				case IRLinearizedLocationType.ArrayElement:
					{
						ArrayElement.ElementType.Resolve(ref ArrayElement.ElementType, ParentInstruction.ParentMethod.ParentType.GenericParameters, ParentInstruction.ParentMethod.GenericParameters);
						ArrayElement.ArrayLocation.Resolve();
						ArrayElement.IndexLocation.Resolve();
						break;
					}
				case IRLinearizedLocationType.ArrayElementAddress:
					{
						ArrayElementAddress.ElementType.Resolve(ref ArrayElementAddress.ElementType, ParentInstruction.ParentMethod.ParentType.GenericParameters, ParentInstruction.ParentMethod.GenericParameters);
						ArrayElementAddress.ArrayLocation.Resolve();
						ArrayElementAddress.IndexLocation.Resolve();
						break;
					}
				case IRLinearizedLocationType.ArrayLength:
					{
						ArrayLength.ArrayLocation.Resolve();
						break;
					}
				case IRLinearizedLocationType.FunctionAddress:
					{
						if (FunctionAddress.InstanceLocation != null) FunctionAddress.InstanceLocation.Resolve();
						FunctionAddress.Method.Resolve(ref FunctionAddress.Method, ParentInstruction.ParentMethod.ParentType.GenericParameters, ParentInstruction.ParentMethod.GenericParameters);
						break;
					}
				case IRLinearizedLocationType.RuntimeHandle:
					{
						if (RuntimeHandle.TargetType != null) RuntimeHandle.TargetType.Resolve(ref RuntimeHandle.TargetType, ParentInstruction.ParentMethod.ParentType.GenericParameters, ParentInstruction.ParentMethod.GenericParameters);
						else if (RuntimeHandle.TargetMethod != null) RuntimeHandle.TargetMethod.Resolve(ref RuntimeHandle.TargetMethod, ParentInstruction.ParentMethod.ParentType.GenericParameters, ParentInstruction.ParentMethod.GenericParameters);
						else if (RuntimeHandle.TargetField != null) RuntimeHandle.TargetField.Resolve(ref RuntimeHandle.TargetField, ParentInstruction.ParentMethod.ParentType.GenericParameters, ParentInstruction.ParentMethod.GenericParameters);
						break;
					}
				case IRLinearizedLocationType.Phi:
					{
						Phi.SourceLocations.ForEach(l => l.Resolve());
						break;
					}

				default: throw new Exception();
			}
		}

		public void RetargetLocals(IRLocal[] pCurrentIterations)
		{
			switch (Type)
			{
				case IRLinearizedLocationType.Local: Local.LocalIndex = pCurrentIterations[Local.LocalIndex].Index; break;
				case IRLinearizedLocationType.LocalAddress: LocalAddress.LocalIndex = pCurrentIterations[LocalAddress.LocalIndex].Index; break;
				case IRLinearizedLocationType.ArrayElement:
					ArrayElement.ArrayLocation.RetargetLocals(pCurrentIterations);
					ArrayElement.IndexLocation.RetargetLocals(pCurrentIterations);
					break;
				case IRLinearizedLocationType.ArrayElementAddress:
					ArrayElementAddress.ArrayLocation.RetargetLocals(pCurrentIterations);
					ArrayElementAddress.IndexLocation.RetargetLocals(pCurrentIterations);
					break;
				case IRLinearizedLocationType.ArrayLength: ArrayLength.ArrayLocation.RetargetLocals(pCurrentIterations); break;
				case IRLinearizedLocationType.Field: Field.FieldLocation.RetargetLocals(pCurrentIterations); break;
				case IRLinearizedLocationType.FieldAddress: FieldAddress.FieldLocation.RetargetLocals(pCurrentIterations); break;
				case IRLinearizedLocationType.Indirect: Indirect.AddressLocation.RetargetLocals(pCurrentIterations); break;
				case IRLinearizedLocationType.FunctionAddress: if (FunctionAddress.InstanceLocation != null) FunctionAddress.InstanceLocation.RetargetLocals(pCurrentIterations); break;
				case IRLinearizedLocationType.Phi: Phi.SourceLocations.ForEach(l => l.RetargetLocals(pCurrentIterations)); break;
				default: break;
			}
		}

		public struct LocalLifetime
		{
			public int Birth;
			public int Death;

			public override string ToString()
			{
				return Birth.ToString() + " - " + Death.ToString();
			}
		}

		public void SetParentInstruction(IRInstruction newParent)
		{
			ParentInstruction = newParent;
			switch (Type)
			{
				case IRLinearizedLocationType.Field:
					Field.FieldLocation.SetParentInstruction(newParent);
					break;
				case IRLinearizedLocationType.FieldAddress:
					FieldAddress.FieldLocation.SetParentInstruction(newParent);
					break;
				case IRLinearizedLocationType.Indirect:
					Indirect.AddressLocation.SetParentInstruction(newParent);
					break;
				case IRLinearizedLocationType.ArrayElement:
					ArrayElement.ArrayLocation.SetParentInstruction(newParent);
					ArrayElement.IndexLocation.SetParentInstruction(newParent);
					break;
				case IRLinearizedLocationType.ArrayElementAddress:
					ArrayElementAddress.ArrayLocation.SetParentInstruction(newParent);
					ArrayElementAddress.IndexLocation.SetParentInstruction(newParent);
					break;
				case IRLinearizedLocationType.ArrayLength:
					ArrayLength.ArrayLocation.SetParentInstruction(newParent);
					break;
				case IRLinearizedLocationType.FunctionAddress:
					if (FunctionAddress.InstanceLocation != null)
						FunctionAddress.InstanceLocation.SetParentInstruction(newParent);
					break;
				case IRLinearizedLocationType.Phi:
					Phi.SourceLocations.ForEach(l => l.SetParentInstruction(newParent));
					break;
				default: break;
			}
		}

		public void MapLocals(LocalLifetime[] lives)
		{
			LocalLifetime liv;
			switch (Type)
			{
				case IRLinearizedLocationType.Local:
					liv = lives[Local.LocalIndex];
					if (liv.Birth > ParentInstruction.IRIndex)
						liv.Birth = ParentInstruction.IRIndex;
					if (liv.Death < ParentInstruction.IRIndex)
						liv.Death = ParentInstruction.IRIndex;
					lives[Local.LocalIndex] = liv;
					break;
				case IRLinearizedLocationType.LocalAddress:
					liv = lives[LocalAddress.LocalIndex];
					if (liv.Birth > ParentInstruction.IRIndex)
						liv.Birth = ParentInstruction.IRIndex;
					if (liv.Death < ParentInstruction.IRIndex)
						liv.Death = ParentInstruction.IRIndex;
					lives[LocalAddress.LocalIndex] = liv;
					break;
				case IRLinearizedLocationType.Field:
					Field.FieldLocation.MapLocals(lives);
					break;
				case IRLinearizedLocationType.FieldAddress:
					FieldAddress.FieldLocation.MapLocals(lives);
					break;
				case IRLinearizedLocationType.Indirect:
					Indirect.AddressLocation.MapLocals(lives);
					break;
				case IRLinearizedLocationType.ArrayElement:
					ArrayElement.ArrayLocation.MapLocals(lives);
					ArrayElement.IndexLocation.MapLocals(lives);
					break;
				case IRLinearizedLocationType.ArrayElementAddress:
					ArrayElementAddress.ArrayLocation.MapLocals(lives);
					ArrayElementAddress.IndexLocation.MapLocals(lives);
					break;
				case IRLinearizedLocationType.ArrayLength:
					ArrayLength.ArrayLocation.MapLocals(lives);
					break;
				case IRLinearizedLocationType.FunctionAddress:
					if (FunctionAddress.InstanceLocation != null)
						FunctionAddress.InstanceLocation.MapLocals(lives);
					break;
				case IRLinearizedLocationType.Phi:
					Phi.SourceLocations.ForEach(l => l.MapLocals(lives));
					break;
			}
		}

		public IRType GetTypeOfLocation()
		{
			switch (Type)
			{
				case IRLinearizedLocationType.Null: return ParentInstruction.ParentMethod.Assembly.AppDomain.System_Object;
				case IRLinearizedLocationType.Local: return ParentInstruction.ParentMethod.Locals[Local.LocalIndex].Type;
				case IRLinearizedLocationType.LocalAddress: return ParentInstruction.ParentMethod.Locals[LocalAddress.LocalIndex].Type.GetManagedPointerType();
				case IRLinearizedLocationType.Parameter: return ParentInstruction.ParentMethod.Parameters[Parameter.ParameterIndex].Type;
				case IRLinearizedLocationType.ParameterAddress: return ParentInstruction.ParentMethod.Parameters[ParameterAddress.ParameterIndex].Type.GetManagedPointerType();
				case IRLinearizedLocationType.ConstantI4: return ParentInstruction.ParentMethod.Assembly.AppDomain.System_Int32;
				case IRLinearizedLocationType.ConstantI8: return ParentInstruction.ParentMethod.Assembly.AppDomain.System_Int64;
				case IRLinearizedLocationType.ConstantR4: return ParentInstruction.ParentMethod.Assembly.AppDomain.System_Single;
				case IRLinearizedLocationType.ConstantR8: return ParentInstruction.ParentMethod.Assembly.AppDomain.System_Double;
				case IRLinearizedLocationType.Field: return Field.Field.Type;
				case IRLinearizedLocationType.FieldAddress: return FieldAddress.Field.Type.GetManagedPointerType();
				case IRLinearizedLocationType.StaticField: return StaticField.Field.Type;
				case IRLinearizedLocationType.StaticFieldAddress: return StaticFieldAddress.Field.Type.GetManagedPointerType();
				case IRLinearizedLocationType.Indirect: return Indirect.Type;
				case IRLinearizedLocationType.SizeOf: return ParentInstruction.ParentMethod.Assembly.AppDomain.System_Int32;
				case IRLinearizedLocationType.ArrayElement: return ArrayElement.ElementType;
				case IRLinearizedLocationType.ArrayElementAddress: return ArrayElementAddress.ElementType.GetManagedPointerType();
				case IRLinearizedLocationType.ArrayLength: return ParentInstruction.ParentMethod.Assembly.AppDomain.System_Int32;
				case IRLinearizedLocationType.FunctionAddress: return ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr;
				case IRLinearizedLocationType.RuntimeHandle: return RuntimeHandle.HandleType;
				case IRLinearizedLocationType.String: return ParentInstruction.ParentMethod.Assembly.AppDomain.System_String;
				case IRLinearizedLocationType.Phi: return Phi.SourceLocations[0].GetTypeOfLocation();
				default: throw new InvalidOperationException();
			}
		}

		private static Dictionary<string, StringLiteralEmittableDataItem> KnownStrings = new Dictionary<string, StringLiteralEmittableDataItem>();
		private static StringLiteralEmittableDataItem GetStringLiteral(LIRCompileUnit cu, string val)
		{
			StringLiteralEmittableDataItem r;
			if (!KnownStrings.TryGetValue(val, out r))
			{
				KnownStrings.Add(val, r = new StringLiteralEmittableDataItem(val));
				cu.AddData(r);
			}
			return r;
		}

		public sealed class StringLiteralEmittableDataItem : EmittableData
		{
			private string Value;
			private Label mLabel = new Label("StringLiteral");
			public override Label Label { get { return mLabel; } }

			public StringLiteralEmittableDataItem(string str)
			{
				this.Value = str;
			}

			public override byte[] GetData(EmissionContext c)
			{
				return System.Text.Encoding.Unicode.GetBytes(Value);
			}

			public override string ToString()
			{
				return string.Format("StringLiteralData(\"{0}\")", 
					Value
					.Replace("\\", "\\\\")
					.Replace("\r", "\\r")
					.Replace("\n", "\\n")
					.Replace("\t", "\\t")
					.Replace("\"", "\\\"")
				);
			}
		}

		public sealed class StaticConstructorCalledEmittableDataItem : EmittableData
		{
			private string TypeName;
			private Label mLabel = new Label("StaticConstructorCalled");
			public override Label Label { get { return mLabel; } }

			public StaticConstructorCalledEmittableDataItem(IRType tp)
			{
				this.TypeName = tp.ToString();
			}

			public override byte[] GetData(EmissionContext c)
			{
				return new byte[1];
			}

			public override string ToString()
			{
				return string.Format("StaticConstructorCalled({0})", TypeName);
			}
		}

		public static Dictionary<IRType, StaticConstructorCalledEmittableDataItem> KnownStaticConstructors = new Dictionary<IRType, StaticConstructorCalledEmittableDataItem>();
		public static void EmitStaticConstructorCheck(LIRMethod m, IRType targetType, IRType curType, bool forceEmit = false)
		{
			if (targetType.HasStaticConstructor && (forceEmit || targetType != curType))
			{
				StaticConstructorCalledEmittableDataItem d = null;
				if (!KnownStaticConstructors.TryGetValue(targetType, out d))
				{
					d = new StaticConstructorCalledEmittableDataItem(targetType);
					m.CompileUnit.AddData(d);
					KnownStaticConstructors.Add(targetType, d);
				}
				new LIRInstructions.Comment(m, "Static Constructor Check");
				var c = m.RequestLocal(targetType.Assembly.AppDomain.System_Boolean);
				new LIRInstructions.Move(m, new Indirect(d.Label), c, targetType.Assembly.AppDomain.System_Boolean);
				Label called = new Label();
				new LIRInstructions.BranchTrue(m, c, called);
				m.ReleaseLocal(c);
				new LIRInstructions.Move(m, (LIRImm)1, new Indirect(d.Label), targetType.Assembly.AppDomain.System_Boolean);
				new LIRInstructions.Call(m, targetType.StaticConstructor);
				m.MarkLabel(called);
			}
		}

		public void LoadTo(LIRMethod pParent, IDestination pDestination)
		{
			switch (Type)
			{
				case IRLinearizedLocationType.Null:
					new LIRInstructions.Move(pParent, (LIRImm)0, pDestination, ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					break;
				case IRLinearizedLocationType.Local:
					new LIRInstructions.Move(pParent, pParent.Locals[Local.LocalIndex], pDestination, pParent.Locals[Local.LocalIndex].Type);
					break;
				case IRLinearizedLocationType.Parameter:
					new LIRInstructions.Move(pParent, pParent.Parameters[Parameter.ParameterIndex], pDestination, pParent.Parameters[Parameter.ParameterIndex].Type);
					break;
				case IRLinearizedLocationType.ConstantI4:
					new LIRInstructions.Move(pParent, (LIRImm)ConstantI4.Value, pDestination, ParentInstruction.ParentMethod.Assembly.AppDomain.System_Int32);
					break;
				case IRLinearizedLocationType.ConstantI8:
					new LIRInstructions.Move(pParent, (LIRImm)ConstantI8.Value, pDestination, ParentInstruction.ParentMethod.Assembly.AppDomain.System_Int64);
					break;
				case IRLinearizedLocationType.ConstantR4:
					new LIRInstructions.Move(pParent, (LIRImm)ConstantR4.Value, pDestination, ParentInstruction.ParentMethod.Assembly.AppDomain.System_Single);
					break;
				case IRLinearizedLocationType.ConstantR8:
					new LIRInstructions.Move(pParent, (LIRImm)ConstantR8.Value, pDestination, ParentInstruction.ParentMethod.Assembly.AppDomain.System_Double);
					break;
				case IRLinearizedLocationType.SizeOf:
					new LIRInstructions.Move(pParent, (LIRImm)SizeOf.Type.StackSize, pDestination, ParentInstruction.ParentMethod.Assembly.AppDomain.System_Int32);
					break;
				case IRLinearizedLocationType.Field:
				{
					var obj = pParent.RequestLocal(Field.FieldLocation.GetTypeOfLocation());
					Field.FieldLocation.LoadTo(pParent, obj);
					var foff = pParent.RequestLocal(ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					new LIRInstructions.Math(pParent, obj, (LIRImm)Field.Field.Offset, foff, LIRInstructions.MathOperation.Add, ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					pParent.ReleaseLocal(obj);
					new LIRInstructions.Move(pParent, new Indirect(foff), pDestination, Field.Field.Type);
					pParent.ReleaseLocal(foff);
					break;
				}
				case IRLinearizedLocationType.FieldAddress:
				{
					var obj = pParent.RequestLocal(FieldAddress.FieldLocation.GetTypeOfLocation());
					FieldAddress.FieldLocation.LoadTo(pParent, obj);
					var foff = pParent.RequestLocal(ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					new LIRInstructions.Math(pParent, obj, (LIRImm)FieldAddress.Field.Offset, foff, LIRInstructions.MathOperation.Add, ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					pParent.ReleaseLocal(obj);
					new LIRInstructions.Move(pParent, foff, pDestination, FieldAddress.Field.Type.GetManagedPointerType());
					pParent.ReleaseLocal(foff);
					break;
				}
				case IRLinearizedLocationType.Indirect:
				{
					var obj = pParent.RequestLocal(Indirect.AddressLocation.GetTypeOfLocation());
					Indirect.AddressLocation.LoadTo(pParent, obj);
					new LIRInstructions.Move(pParent, new Indirect(obj), pDestination, Indirect.Type);
					pParent.ReleaseLocal(obj);
					break;
				}
				case IRLinearizedLocationType.ArrayElement:
				{
					var arr = pParent.RequestLocal(ArrayElement.ArrayLocation.GetTypeOfLocation());
					var idx = pParent.RequestLocal(ArrayElement.IndexLocation.GetTypeOfLocation());
					ArrayElement.ArrayLocation.LoadTo(pParent, arr);
					ArrayElement.IndexLocation.LoadTo(pParent, idx);
					var off = pParent.RequestLocal(ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					new LIRInstructions.Math(pParent, idx, (LIRImm)ArrayElement.ElementType.StackSize, off, LIRInstructions.MathOperation.Multiply, ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					pParent.ReleaseLocal(idx);
					new LIRInstructions.Math(pParent, off, (LIRImm)0x04, off, LIRInstructions.MathOperation.Add, ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					new LIRInstructions.Math(pParent, off, arr, off, LIRInstructions.MathOperation.Add, ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					pParent.ReleaseLocal(arr);
					new LIRInstructions.Move(pParent, new Indirect(off), pDestination, ArrayElement.ElementType);
					pParent.ReleaseLocal(off);
					break;
				}
				case IRLinearizedLocationType.ArrayElementAddress:
				{
					var arr = pParent.RequestLocal(ArrayElementAddress.ArrayLocation.GetTypeOfLocation());
					var idx = pParent.RequestLocal(ArrayElementAddress.IndexLocation.GetTypeOfLocation());
					ArrayElementAddress.ArrayLocation.LoadTo(pParent, arr);
					ArrayElementAddress.IndexLocation.LoadTo(pParent, idx);
					var off = pParent.RequestLocal(ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					new LIRInstructions.Math(pParent, idx, (LIRImm)ArrayElementAddress.ElementType.StackSize, off, LIRInstructions.MathOperation.Multiply, ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					pParent.ReleaseLocal(idx);
					new LIRInstructions.Math(pParent, off, (LIRImm)0x04, off, LIRInstructions.MathOperation.Add, ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					new LIRInstructions.Math(pParent, off, arr, off, LIRInstructions.MathOperation.Add, ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					pParent.ReleaseLocal(arr);
					new LIRInstructions.Move(pParent, off, pDestination, ArrayElementAddress.ElementType.GetManagedPointerType());
					pParent.ReleaseLocal(off);
					break;
				}
				case IRLinearizedLocationType.ArrayLength:
				{
					var arr = pParent.RequestLocal(ArrayLength.ArrayLocation.GetTypeOfLocation());
					ArrayLength.ArrayLocation.LoadTo(pParent, arr);
					new LIRInstructions.Move(pParent, new Indirect(arr), pDestination, ParentInstruction.ParentMethod.Assembly.AppDomain.System_Int32);
					pParent.ReleaseLocal(arr);
					break;
				}
				case IRLinearizedLocationType.LocalAddress:
					new LIRInstructions.Move(pParent, pParent.Locals[Local.LocalIndex].AddressOf(), pDestination, pParent.Locals[Local.LocalIndex].Type);
					break;
				case IRLinearizedLocationType.ParameterAddress:
					new LIRInstructions.Move(pParent, pParent.Parameters[Parameter.ParameterIndex].AddressOf(), pDestination, pParent.Parameters[Parameter.ParameterIndex].Type);
					break;
				case IRLinearizedLocationType.String:
				{
					var dest = pParent.RequestLocal(ParentInstruction.AppDomain.System_String.GetManagedPointerType());
					new LIRInstructions.Move(pParent, dest.AddressOf(), dest, dest.Type);
					var str = GetStringLiteral(pParent.CompileUnit, String.Value);
					var strLb = pParent.RequestLocal(ParentInstruction.AppDomain.System_UIntPtr);
					new LIRInstructions.Move(pParent, str.Label, strLb, strLb.Type);
					new LIRInstructions.Call(pParent, ParentInstruction.AppDomain.System_GC_AllocateStringFromUTF16, new List<ISource>() { strLb, dest }, null);
					pParent.ReleaseLocal(strLb);
					new LIRInstructions.Move(pParent, dest, pDestination, dest.Type);
					pParent.ReleaseLocal(dest);
					break;
				}
				case IRLinearizedLocationType.StaticField:
				{
					EmitStaticConstructorCheck(pParent, StaticField.Field.ParentType, ParentInstruction.ParentMethod.ParentType);
					new LIRInstructions.Move(pParent, new Indirect(StaticField.Field.Label), pDestination, StaticField.Field.Type);
					break;
				}
				case IRLinearizedLocationType.StaticFieldAddress:
				{
					EmitStaticConstructorCheck(pParent, StaticFieldAddress.Field.ParentType, ParentInstruction.ParentMethod.ParentType);
					new LIRInstructions.Move(pParent, StaticFieldAddress.Field.Label, pDestination, StaticField.Field.Type);
					break;
				}
				case IRLinearizedLocationType.FunctionAddress:
				{
					if (FunctionAddress.Virtual)
					{
						if (FunctionAddress.Method.VirtualMethodIndex < 0)
							throw new Exception("The requested method never got laid out in a virtual method tree!");
						if (FunctionAddress.Method.ParentType.IsInterface)
						{
#warning Loading the virtual function address of a method on an interface isn't supported yet!
						}
						else
						{
							var s = pParent.RequestLocal(Indirect.AddressLocation.GetTypeOfLocation());
							Indirect.AddressLocation.LoadTo(pParent, s); // Gets Object Pointer
							new LIRInstructions.Math(pParent, s, (LIRImm)VMConfig.PointerSizeForTarget, s, LIRInstructions.MathOperation.Subtract, s.Type); // Subtract sizeof pointer to get address of TypeData pointer
							new LIRInstructions.Move(pParent, new Indirect(s), s, s.Type); // Get TypeData Pointer
							new LIRInstructions.Move(pParent, new Indirect(s), s, s.Type); // Get VirtualMethodTree Pointer Array
							new LIRInstructions.Math(pParent, s, (LIRImm)(VMConfig.PointerSizeForTarget * FunctionAddress.Method.VirtualMethodIndex), s, LIRInstructions.MathOperation.Add, s.Type); // Get VirtualMethod Pointer Address
							new LIRInstructions.Move(pParent, new Indirect(s), pDestination, s.Type); // Get VirtualMethod Pointer
							pParent.ReleaseLocal(s);
						}
					}
					else
					{
						new LIRInstructions.Move(pParent, FunctionAddress.Method.LIRMethod.Label, pDestination, ParentInstruction.AppDomain.System_UIntPtr);
					}
					break;
				}
				case IRLinearizedLocationType.RuntimeHandle:
				{
					if (RuntimeHandle.HandleType == ParentInstruction.AppDomain.System_RuntimeTypeHandle)
					{
						new LIRInstructions.Move(pParent, RuntimeHandle.TargetType.MetadataLabel, pDestination, RuntimeHandle.HandleType);
					}
					else if (RuntimeHandle.HandleType == ParentInstruction.AppDomain.System_RuntimeMethodHandle)
					{
						new LIRInstructions.Move(pParent, RuntimeHandle.TargetMethod.MetadataLabel, pDestination, RuntimeHandle.HandleType);
					}
					else if (RuntimeHandle.HandleType == ParentInstruction.AppDomain.System_RuntimeFieldHandle)
					{
						new LIRInstructions.Move(pParent, RuntimeHandle.TargetField.MetadataLabel, pDestination, RuntimeHandle.HandleType);
					}
					else
					{
						throw new Exception("Unknown RuntimeHandle HandleType!");
					}
					break;
				}

				case IRLinearizedLocationType.Phi:
					throw new Exception("All phi's should have been eliminated by this point!");
				default:
					throw new Exception("Unknown IRLinearizedLocationType!");
			}
		}

		public void LoadAddressTo(LIRMethod pParent, IDestination pDestination)
		{
			switch (Type)
			{
				case IRLinearizedLocationType.Local:
					new LIRInstructions.Move(pParent, pParent.Locals[Local.LocalIndex].AddressOf(), pDestination, pParent.Locals[Local.LocalIndex].Type);
					break;
				case IRLinearizedLocationType.Parameter:
					new LIRInstructions.Move(pParent, pParent.Parameters[Parameter.ParameterIndex].AddressOf(), pDestination, pParent.Parameters[Parameter.ParameterIndex].Type);
					break;
				case IRLinearizedLocationType.Indirect:
					Indirect.AddressLocation.LoadTo(pParent, pDestination);
					break;
				case IRLinearizedLocationType.Field:
				{
					var obj = pParent.RequestLocal(Field.FieldLocation.GetTypeOfLocation());
					Field.FieldLocation.LoadTo(pParent, obj);
					var foff = pParent.RequestLocal(ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					new LIRInstructions.Math(pParent, obj, (LIRImm)Field.Field.Offset, foff, LIRInstructions.MathOperation.Add, ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					pParent.ReleaseLocal(obj);
					new LIRInstructions.Move(pParent, foff, pDestination, Field.Field.Type.GetManagedPointerType());
					pParent.ReleaseLocal(foff);
					break;
				}
				case IRLinearizedLocationType.ArrayElement:
				{
					var arr = pParent.RequestLocal(ArrayElement.ArrayLocation.GetTypeOfLocation());
					var idx = pParent.RequestLocal(ArrayElement.IndexLocation.GetTypeOfLocation());
					ArrayElement.ArrayLocation.LoadTo(pParent, arr);
					ArrayElement.IndexLocation.LoadTo(pParent, idx);
					var off = pParent.RequestLocal(ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					new LIRInstructions.Math(pParent, idx, (LIRImm)ArrayElement.ElementType.StackSize, off, LIRInstructions.MathOperation.Multiply, ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					pParent.ReleaseLocal(idx);
					new LIRInstructions.Math(pParent, off, (LIRImm)0x04, off, LIRInstructions.MathOperation.Add, ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					new LIRInstructions.Math(pParent, off, arr, off, LIRInstructions.MathOperation.Add, ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					pParent.ReleaseLocal(arr);
					new LIRInstructions.Move(pParent, off, pDestination, ArrayElement.ElementType.GetManagedPointerType());
					pParent.ReleaseLocal(off);
					break;
				}
				case IRLinearizedLocationType.StaticField:
				{
					EmitStaticConstructorCheck(pParent, StaticField.Field.ParentType, ParentInstruction.ParentMethod.ParentType);
					new LIRInstructions.Move(pParent, StaticField.Field.Label, pDestination, StaticField.Field.Type);
					break;
				}

#warning Finish the rest of these case statements
				case IRLinearizedLocationType.Null:
				case IRLinearizedLocationType.LocalAddress:
				case IRLinearizedLocationType.ParameterAddress:
				case IRLinearizedLocationType.ConstantI4:
				case IRLinearizedLocationType.ConstantI8:
				case IRLinearizedLocationType.ConstantR4:
				case IRLinearizedLocationType.ConstantR8:
				case IRLinearizedLocationType.FieldAddress:
				case IRLinearizedLocationType.StaticFieldAddress:
				case IRLinearizedLocationType.ArrayElementAddress:
				case IRLinearizedLocationType.ArrayLength:
				case IRLinearizedLocationType.FunctionAddress:
				case IRLinearizedLocationType.RuntimeHandle:
				case IRLinearizedLocationType.String:
				case IRLinearizedLocationType.SizeOf:
					break;
					//throw new Exception("It's not possible to load the address of these!");
				case IRLinearizedLocationType.Phi:
					throw new Exception("All phi's should have been eliminated by this point!");
				default:
					throw new Exception("Unknown IRLinearizedLocationType!");
			}
		}

		public void StoreTo(LIRMethod pParent, ISource pSource)
		{
			switch (Type)
			{
				case IRLinearizedLocationType.Local:
					new LIRInstructions.Move(pParent, pSource, pParent.Locals[Local.LocalIndex], pParent.Locals[Local.LocalIndex].Type);
					break;
				case IRLinearizedLocationType.Parameter:
					new LIRInstructions.Move(pParent, pSource, pParent.Parameters[Parameter.ParameterIndex], pParent.Parameters[Parameter.ParameterIndex].Type);
					break;
				case IRLinearizedLocationType.Field:
				{
					var obj = pParent.RequestLocal(Field.FieldLocation.GetTypeOfLocation());
					Field.FieldLocation.LoadTo(pParent, obj);
					var foff = pParent.RequestLocal(ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					new LIRInstructions.Math(pParent, obj, (LIRImm)Field.Field.Offset, foff, LIRInstructions.MathOperation.Add, ParentInstruction.AppDomain.System_UIntPtr);
					pParent.ReleaseLocal(obj);
					new LIRInstructions.Move(pParent, pSource, new Indirect(foff), Field.Field.Type);
					pParent.ReleaseLocal(foff);
					break;
				}
				case IRLinearizedLocationType.Indirect:
				{
					var obj = pParent.RequestLocal(Indirect.AddressLocation.GetTypeOfLocation());
					Indirect.AddressLocation.LoadTo(pParent, obj);
					new LIRInstructions.Move(pParent, pSource, new Indirect(obj), Indirect.Type);
					pParent.ReleaseLocal(obj);
					break;
				}
				case IRLinearizedLocationType.ArrayElement:
				{
					var arr = pParent.RequestLocal(ArrayElement.ArrayLocation.GetTypeOfLocation());
					var idx = pParent.RequestLocal(ArrayElement.IndexLocation.GetTypeOfLocation());
					ArrayElement.ArrayLocation.LoadTo(pParent, arr);
					ArrayElement.IndexLocation.LoadTo(pParent, idx);
					var off = pParent.RequestLocal(ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					new LIRInstructions.Math(pParent, idx, (LIRImm)ArrayElement.ElementType.StackSize, off, LIRInstructions.MathOperation.Multiply, ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					pParent.ReleaseLocal(idx);
					new LIRInstructions.Math(pParent, off, (LIRImm)0x04, off, LIRInstructions.MathOperation.Add, ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					new LIRInstructions.Math(pParent, off, arr, off, LIRInstructions.MathOperation.Add, ParentInstruction.ParentMethod.Assembly.AppDomain.System_UIntPtr);
					pParent.ReleaseLocal(arr);
					new LIRInstructions.Move(pParent, pSource, new Indirect(off), ArrayElement.ElementType);
					pParent.ReleaseLocal(off);
					break;
				}
				case IRLinearizedLocationType.StaticField:
				{
					EmitStaticConstructorCheck(pParent, StaticField.Field.ParentType, ParentInstruction.ParentMethod.ParentType);
					new LIRInstructions.Move(pParent, pSource, new Indirect(StaticField.Field.Label), StaticField.Field.Type);
					break;
				}


				case IRLinearizedLocationType.Null:
				case IRLinearizedLocationType.LocalAddress:
				case IRLinearizedLocationType.ParameterAddress:
				case IRLinearizedLocationType.ConstantI4:
				case IRLinearizedLocationType.ConstantI8:
				case IRLinearizedLocationType.ConstantR4:
				case IRLinearizedLocationType.ConstantR8:
				case IRLinearizedLocationType.FieldAddress:
				case IRLinearizedLocationType.StaticFieldAddress:
				case IRLinearizedLocationType.ArrayElementAddress:
				case IRLinearizedLocationType.ArrayLength:
				case IRLinearizedLocationType.FunctionAddress:
				case IRLinearizedLocationType.RuntimeHandle:
				case IRLinearizedLocationType.String:
				case IRLinearizedLocationType.SizeOf:
					throw new Exception("It's not possible to store to these!");
				case IRLinearizedLocationType.Phi:
					throw new Exception("All phi's should have been eliminated by this point!");
				default:
					throw new Exception("Unknown IRLinearizedLocationType!");
			}
		}

		public void Dump(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("#" + mTempID + " Parent: " + ParentInstruction.IRIndex);
			switch (Type)
			{
				case IRLinearizedLocationType.Null: break;
				case IRLinearizedLocationType.Local:
					pWriter.WriteLine("LocalIndex {0}", Local.LocalIndex);
					break;
				case IRLinearizedLocationType.LocalAddress:
					pWriter.WriteLine("LocalIndex {0}", LocalAddress.LocalIndex);
					break;
				case IRLinearizedLocationType.Parameter:
					pWriter.WriteLine("ParameterIndex {0}", Parameter.ParameterIndex);
					break;
				case IRLinearizedLocationType.ParameterAddress:
					pWriter.WriteLine("ParameterIndex {0}", ParameterAddress.ParameterIndex);
					break;
				case IRLinearizedLocationType.ConstantI4:
					pWriter.WriteLine("Value {0}", ConstantI4.Value);
					break;
				case IRLinearizedLocationType.ConstantI8:
					pWriter.WriteLine("Value {0}", ConstantI8.Value);
					break;
				case IRLinearizedLocationType.ConstantR4:
					pWriter.WriteLine("Value {0}", ConstantR4.Value);
					break;
				case IRLinearizedLocationType.ConstantR8:
					pWriter.WriteLine("Value {0}", ConstantR8.Value);
					break;
				case IRLinearizedLocationType.Field:
					pWriter.WriteLine("Field {0}", Field.Field.ToString());
					pWriter.WriteLine("FieldLocation {0}", Field.FieldLocation.Type);
					pWriter.WriteLine("{");
					pWriter.Indent++;
					Field.FieldLocation.Dump(pWriter);
					pWriter.Indent--;
					pWriter.WriteLine("}");
					break;
				case IRLinearizedLocationType.FieldAddress:
					pWriter.WriteLine("Field {0}", FieldAddress.Field.ToString());
					pWriter.WriteLine("FieldLocation {0}", FieldAddress.FieldLocation.Type);
					pWriter.WriteLine("{");
					pWriter.Indent++;
					FieldAddress.FieldLocation.Dump(pWriter);
					pWriter.Indent--;
					pWriter.WriteLine("}");
					break;
				case IRLinearizedLocationType.StaticField:
					pWriter.WriteLine("Field {0}", StaticField.Field.ToString());
					break;
				case IRLinearizedLocationType.StaticFieldAddress:
					pWriter.WriteLine("Field {0}", StaticFieldAddress.Field.ToString());
					break;
				case IRLinearizedLocationType.Indirect:
					pWriter.WriteLine("Type {0}", Indirect.Type.ToString());
					pWriter.WriteLine("AddressLocation {0}", Indirect.AddressLocation.Type);
					pWriter.WriteLine("{");
					pWriter.Indent++;
					Indirect.AddressLocation.Dump(pWriter);
					pWriter.Indent--;
					pWriter.WriteLine("}");
					break;
				case IRLinearizedLocationType.SizeOf:
					pWriter.WriteLine("Type {0}", SizeOf.Type.ToString());
					break;
				case IRLinearizedLocationType.ArrayElement:
					pWriter.WriteLine("ElementType {0}", ArrayElement.ElementType.ToString());
					pWriter.WriteLine("ArrayLocation {0}", ArrayElement.ArrayLocation.Type);
					pWriter.WriteLine("{");
					pWriter.Indent++;
					ArrayElement.ArrayLocation.Dump(pWriter);
					pWriter.Indent--;
					pWriter.WriteLine("}");
					pWriter.WriteLine("IndexLocation {0}", ArrayElement.IndexLocation.Type);
					pWriter.WriteLine("{");
					pWriter.Indent++;
					ArrayElement.IndexLocation.Dump(pWriter);
					pWriter.Indent--;
					pWriter.WriteLine("}");
					break;
				case IRLinearizedLocationType.ArrayElementAddress:
					pWriter.WriteLine("ElementType {0}", ArrayElementAddress.ElementType.ToString());
					pWriter.WriteLine("ArrayLocation {0}", ArrayElementAddress.ArrayLocation.Type);
					pWriter.WriteLine("{");
					pWriter.Indent++;
					ArrayElementAddress.ArrayLocation.Dump(pWriter);
					pWriter.Indent--;
					pWriter.WriteLine("}");
					pWriter.WriteLine("IndexLocation {0}", ArrayElementAddress.IndexLocation.Type);
					pWriter.WriteLine("{");
					pWriter.Indent++;
					ArrayElementAddress.IndexLocation.Dump(pWriter);
					pWriter.Indent--;
					pWriter.WriteLine("}");
					break;
				case IRLinearizedLocationType.ArrayLength:
					pWriter.WriteLine("ArrayLocation {0}", ArrayLength.ArrayLocation.Type);
					pWriter.WriteLine("{");
					pWriter.Indent++;
					ArrayLength.ArrayLocation.Dump(pWriter);
					pWriter.Indent--;
					pWriter.WriteLine("}");
					break;
				case IRLinearizedLocationType.FunctionAddress:
					if (FunctionAddress.InstanceLocation != null)
					{
						pWriter.WriteLine("InstanceLocation {0}", FunctionAddress.InstanceLocation.Type);
						pWriter.WriteLine("{");
						pWriter.Indent++;
						FunctionAddress.InstanceLocation.Dump(pWriter);
						pWriter.Indent--;
						pWriter.WriteLine("}");
					}
					pWriter.WriteLine("Method {0}", FunctionAddress.Method.ToString());
					pWriter.WriteLine("Virtual {0}", FunctionAddress.Virtual);
					break;
				case IRLinearizedLocationType.RuntimeHandle:
					pWriter.WriteLine("HandleType {0}", RuntimeHandle.HandleType.ToString());
					if (RuntimeHandle.TargetType != null)
						pWriter.WriteLine("TargetType {0}", RuntimeHandle.TargetType.ToString());
					if (RuntimeHandle.TargetMethod != null)
						pWriter.WriteLine("TargetMethod {0}", RuntimeHandle.TargetMethod.ToString());
					if (RuntimeHandle.TargetField != null)
						pWriter.WriteLine("TargetField {0}", RuntimeHandle.TargetField.ToString());
					break;
				case IRLinearizedLocationType.String:
					pWriter.WriteLine("Value {0}", String.Value);
					break;
				case IRLinearizedLocationType.Phi:
					Phi.SourceLocations.ForEach(l => l.Dump(pWriter));
					break;
			}
		}

		public override string ToString()
		{
			switch (Type)
			{
				case IRLinearizedLocationType.Null:
					return "Null";
				case IRLinearizedLocationType.Local:
					return "Local(" + Local.LocalIndex + ")";
				case IRLinearizedLocationType.LocalAddress:
					return "LocalAddress(" + LocalAddress.LocalIndex + ")";
				case IRLinearizedLocationType.Parameter:
					return "Parameter(" + Parameter.ParameterIndex + ")";
				case IRLinearizedLocationType.ParameterAddress:
					return "ParameterAddress(" + ParameterAddress.ParameterIndex + ")";
				case IRLinearizedLocationType.ConstantI4:
					return "ConstantI4(" + ConstantI4.Value + ")";
				case IRLinearizedLocationType.ConstantI8:
					return "ConstantI8(" + ConstantI8.Value + ")";
				case IRLinearizedLocationType.ConstantR4:
					return "ConstantR4(" + ConstantR4.Value + ")";
				case IRLinearizedLocationType.ConstantR8:
					return "ConstantR8(" + ConstantR8.Value + ")";
				case IRLinearizedLocationType.Field:
					return "Field(" + Field.FieldLocation + ", " + Field.Field.ToString(true) + ")";
				case IRLinearizedLocationType.FieldAddress:
					return "FieldAddress(" + FieldAddress.FieldLocation + ", " + FieldAddress.Field.ToString(true) + ")";
				case IRLinearizedLocationType.StaticField:
					return "StaticField(" + StaticField.Field + ")";
				case IRLinearizedLocationType.StaticFieldAddress:
					return "StaticFieldAddress(" + StaticFieldAddress.Field + ")";
				case IRLinearizedLocationType.Indirect:
					return "Indirect(" + Indirect.Type + ", " + Indirect.AddressLocation + ")";
				case IRLinearizedLocationType.SizeOf:
					return "SizeOf(" + SizeOf.Type + ")";
				case IRLinearizedLocationType.ArrayElement:
					return "ArrayElement(" + ArrayElement.ElementType + ", " + ArrayElement.ArrayLocation + ", " + ArrayElement.IndexLocation + ")";
				case IRLinearizedLocationType.ArrayElementAddress:
					return "ArrayElementAddress(" + ArrayElementAddress.ElementType + ", " + ArrayElementAddress.ArrayLocation + ", " + ArrayElementAddress.IndexLocation + ")";
				case IRLinearizedLocationType.ArrayLength:
					return "ArrayLength(" + ArrayLength.ArrayLocation + ")";
				case IRLinearizedLocationType.FunctionAddress:
					return "FunctionAddress(" + FunctionAddress.Virtual + ", " + FunctionAddress.InstanceLocation + ", " + FunctionAddress.Method + ")";
				case IRLinearizedLocationType.RuntimeHandle:
					if (RuntimeHandle.HandleType == ParentInstruction.ParentMethod.Assembly.AppDomain.System_RuntimeFieldHandle)
						return "RuntimeHandle(Field, " + RuntimeHandle.TargetField + ")";
					else if (RuntimeHandle.HandleType == ParentInstruction.ParentMethod.Assembly.AppDomain.System_RuntimeMethodHandle)
						return "RuntimeHandle(Method, " + RuntimeHandle.TargetMethod + ")";
					else if (RuntimeHandle.HandleType == ParentInstruction.ParentMethod.Assembly.AppDomain.System_RuntimeTypeHandle)
						return "RuntimeHandle(Type, " + RuntimeHandle.TargetType + ")";
					else
						throw new Exception("Unknown runtime handle type!");
				case IRLinearizedLocationType.String:
					return "String(\"" + String.Value + "\")";
				case IRLinearizedLocationType.Phi:
					return "Phi(" + string.Join(", ", Phi.SourceLocations) + ")";
				default:
					throw new Exception("Unknown IRLinearizedLocationType!");
			}
		}
	}
}
