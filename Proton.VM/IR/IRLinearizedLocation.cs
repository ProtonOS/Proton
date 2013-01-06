using Proton.LIR;
using LIRInstructions = Proton.LIR.Instructions;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Proton.VM.IR
{
    public class IRLinearizedLocation
    {
        public struct LocalLocationData { public int LocalIndex; }
        public struct LocalAddressLocationData { public int LocalIndex; }
        public struct ParameterLocationData { public uint ParameterIndex; }
        public struct ParameterAddressLocationData { public uint ParameterIndex; }
        public struct ConstantI4LocationData { public int Value; }
        public struct ConstantI8LocationData { public long Value; }
        public struct ConstantR4LocationData { public float Value; }
        public struct ConstantR8LocationData { public double Value; }
        public struct FieldLocationData
        {
            public IRField Field;
            public IRLinearizedLocation FieldLocation;
        }
        public struct FieldAddressLocationData
        {
            public IRField Field;
            public IRLinearizedLocation FieldLocation;
        }
        public struct StaticFieldLocationData { public IRField Field; }
        public struct StaticFieldAddressLocationData { public IRField Field; }
        public struct IndirectLocationData
        {
            public IRType Type;
            public IRLinearizedLocation AddressLocation;
        }
        public struct SizeOfLocationData { public IRType Type; }
        public struct ArrayElementLocationData
        {
            public IRType ElementType;
            public IRLinearizedLocation ArrayLocation;
            public IRLinearizedLocation IndexLocation;
        }
        public struct ArrayElementAddressLocationData
        {
            public IRType ElementType;
            public IRLinearizedLocation ArrayLocation;
            public IRLinearizedLocation IndexLocation;
        }
        public struct ArrayLengthLocationData { public IRLinearizedLocation ArrayLocation; }
        public struct FunctionAddressLocationData
        {
			public IRLinearizedLocation InstanceLocation;
            public IRMethod Method;
            public bool Virtual;
        }
        public struct RuntimeHandleLocationData
        {
            public IRType HandleType;
            public IRType TargetType;
            public IRMethod TargetMethod;
            public IRField TargetField;
        }
        public struct StringLocationData { public string Value; }
		public struct PhiLocationData
		{
			public List<IRLinearizedLocation> SourceLocations;
		}

		private static int sTempID = 0;
		internal int mTempID = 0;

		public IRInstruction ParentInstruction;
        public IRLinearizedLocationType Type;
        public LocalLocationData Local;
        public LocalAddressLocationData LocalAddress;
        public ParameterLocationData Parameter;
        public ParameterAddressLocationData ParameterAddress;
        public ConstantI4LocationData ConstantI4;
        public ConstantI8LocationData ConstantI8;
        public ConstantR4LocationData ConstantR4;
        public ConstantR8LocationData ConstantR8;
        public FieldLocationData Field;
        public FieldAddressLocationData FieldAddress;
        public StaticFieldLocationData StaticField;
        public StaticFieldAddressLocationData StaticFieldAddress;
        public IndirectLocationData Indirect;
        public SizeOfLocationData SizeOf;
        public ArrayElementLocationData ArrayElement;
        public ArrayElementAddressLocationData ArrayElementAddress;
        public ArrayLengthLocationData ArrayLength;
        public FunctionAddressLocationData FunctionAddress;
        public RuntimeHandleLocationData RuntimeHandle;
        public StringLocationData String;
		public PhiLocationData Phi;

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
						liv.Birth = (int)ParentInstruction.IRIndex;
					if (liv.Death < ParentInstruction.IRIndex)
						liv.Death = (int)ParentInstruction.IRIndex;
					lives[Local.LocalIndex] = liv;
					break;
				case IRLinearizedLocationType.LocalAddress:
					liv = lives[LocalAddress.LocalIndex];
					if (liv.Birth > ParentInstruction.IRIndex)
						liv.Birth = (int)ParentInstruction.IRIndex;
					if (liv.Death < ParentInstruction.IRIndex)
						liv.Death = (int)ParentInstruction.IRIndex;
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
				case IRLinearizedLocationType.Parameter: return ParentInstruction.ParentMethod.Parameters[(int)Parameter.ParameterIndex].Type;
				case IRLinearizedLocationType.ParameterAddress: return ParentInstruction.ParentMethod.Parameters[(int)ParameterAddress.ParameterIndex].Type.GetManagedPointerType();
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
				case IRLinearizedLocationType.Phi: return ParentInstruction.ParentMethod.Locals[Phi.SourceLocations[0].Local.LocalIndex].Type;
				default: throw new InvalidOperationException();
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
					new LIRInstructions.Move(pParent, pParent.Parameters[(int)Parameter.ParameterIndex], pDestination, pParent.Parameters[(int)Parameter.ParameterIndex].Type);
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
#warning Finish the rest of these case statements

				//case IRLinearizedLocationType.Phi:
				//	throw new Exception("All phi's should have been eliminated by this point!");

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
					new LIRInstructions.Move(pParent, pSource, pParent.Parameters[(int)Parameter.ParameterIndex], pParent.Parameters[(int)Parameter.ParameterIndex].Type);
					break;
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
					return "Field(" + Field.FieldLocation + ", " + Field.Field + ")";
				case IRLinearizedLocationType.FieldAddress:
					return "FieldAddress(" + FieldAddress.FieldLocation + ", " + FieldAddress.Field + ")";
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
					return "String(" + String.Value + ")";
				case IRLinearizedLocationType.Phi:
					return "Phi(" + string.Join(", ", Phi.SourceLocations) + ")";
				default:
					throw new Exception("Unknown IRLinearizedLocationType!");
			}
		}
	}
}
