using System;
using System.Runtime.InteropServices;

namespace Proton.VM.IR
{
    public class IRLinearizedLocation
    {
        public struct LocalLocationData { public uint LocalIndex; }
        public struct LocalAddressLocationData { public uint LocalIndex; }
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

        public void Resolve(IRGenericParameterCollection typeParams, IRGenericParameterCollection methodParams)
        {
        }
    }
}
