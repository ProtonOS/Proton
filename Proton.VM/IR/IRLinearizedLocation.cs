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
            public bool NoChecksRequired;
        }
        public struct ArrayElementAddressLocationData
        {
            public IRType ElementType;
            public IRLinearizedLocation ArrayLocation;
            public IRLinearizedLocation IndexLocation;
            public bool NoChecksRequired;
        }
        public struct ArrayLengthLocationData { public IRLinearizedLocation ArrayLocation; }
        public struct FunctionAddressLocationData
        {
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

        public IRLinearizedLocation(IRLinearizedLocationType pType) { Type = pType; }
        public IRLinearizedLocation(IRLinearizedLocation pLinearizedTarget)
        {
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
                case IRLinearizedLocationType.Field: Field = pLinearizedTarget.Field; break;
                case IRLinearizedLocationType.FieldAddress: FieldAddress = pLinearizedTarget.FieldAddress; break;
                case IRLinearizedLocationType.StaticField: StaticField = pLinearizedTarget.StaticField; break;
                case IRLinearizedLocationType.StaticFieldAddress: StaticFieldAddress = pLinearizedTarget.StaticFieldAddress; break;
                case IRLinearizedLocationType.Indirect: Indirect = pLinearizedTarget.Indirect; break;
                case IRLinearizedLocationType.SizeOf: SizeOf = pLinearizedTarget.SizeOf; break;
                case IRLinearizedLocationType.ArrayElement: ArrayElement = pLinearizedTarget.ArrayElement; break;
                case IRLinearizedLocationType.ArrayElementAddress: ArrayElementAddress = pLinearizedTarget.ArrayElementAddress; break;
                case IRLinearizedLocationType.ArrayLength: ArrayLength = pLinearizedTarget.ArrayLength; break;
                case IRLinearizedLocationType.FunctionAddress: FunctionAddress = pLinearizedTarget.FunctionAddress; break;
                case IRLinearizedLocationType.RuntimeHandle: RuntimeHandle = pLinearizedTarget.RuntimeHandle; break;
                case IRLinearizedLocationType.String: String = pLinearizedTarget.String; break;
                default: throw new ArgumentException("Type");
            }
        }

        public IRLinearizedLocation Clone() { return new IRLinearizedLocation(this); }

        public void Resolve(IRGenericParameterList typeParams, IRGenericParameterList methodParams)
        {
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
				default: break;
			}
		}

		public void Dump(IndentableStreamWriter pWriter)
		{
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
					Field.FieldLocation.Dump(pWriter);
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
					pWriter.WriteLine("NoChecksRequired {0}", ArrayElement.NoChecksRequired);
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
					pWriter.WriteLine("NoChecksRequired {0}", ArrayElementAddress.NoChecksRequired);
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
			}
		}
	}
}
