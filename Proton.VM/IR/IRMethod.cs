﻿using Proton.Metadata;
using Proton.Metadata.Tables;
using Proton.Metadata.Signatures;
using Proton.VM.IL;
using Proton.VM.IR.Instructions;
using System;
using System.Linq;
using System.Collections;
using System.Collections.Generic;
using System.Text;

namespace Proton.VM.IR
{
    public sealed class IRMethod
    {
        public IRAssembly Assembly = null;

        public string Name = null;

		public bool IsStatic { get; set; }

        public IRType ParentType = null;
        public IRType ReturnType = null;
        public readonly List<IRParameter> Parameters = new List<IRParameter>();
        public readonly List<IRLocal> Locals = new List<IRLocal>();
        public readonly IRInstructionList Instructions = new IRInstructionList();

        private bool? mResolvedCache;
        public bool Resolved
        {
            get
            {
                if (mResolvedCache != null)
                    return mResolvedCache.Value;

				if (ReturnType != null && ReturnType != ParentType)
				{
					if (!ReturnType.Resolved) return false;
				}
				if (!Parameters.Where(p => p.Type != ParentType).TrueForAll(p => p.Resolved)) return false;
				if (!Locals.Where(p => p.Type != ParentType).TrueForAll(l => l.Resolved)) return false;
				if (!Instructions.TrueForAll(i => i.Resolved)) return false;

				mResolvedCache = true;
                return true;
            }
        }

		public void Resolve(ref IRMethod selfReference)
		{
			if (!Resolved)
			{
				if (IsGeneric)
				{
					// This will eventually need to get the instantiation of this method.
				}
				else
				{
					Substitute(IRGenericParameterList.Empty);
				}
			}
		}

        public void Substitute(IRGenericParameterList methodParams)
        {
			GenericParameters.Substitute(IRGenericParameterList.Empty, methodParams);
			if (ReturnType != null)
				ReturnType.Resolve(ref ReturnType, ParentType.GenericParameters, GenericParameters);
			Parameters.ForEach(p => p.Substitute());
			Locals.ForEach(l => l.Substitute());
			Instructions.ForEach(i => i.Substitute());
        }

		/// <summary>
		/// This creates a shallow clone of this method, but
		/// does a deep clone of it's instructions, parameters, and locals.
		/// </summary>
		/// <param name="newParent">The parent for the new method.</param>
		/// <returns>The clone of this method.</returns>
		public IRMethod Clone(IRType newParent)
		{
			IRMethod m = new IRMethod(this.Assembly);

			m.GenericMethod = this.GenericMethod;
			m.GenericParameters.AddRange(this.GenericParameters);
			this.Instructions.ForEach(i => m.Instructions.Add(i.Clone(m)));
			this.Locals.ForEach(l => m.Locals.Add(l.Clone(m)));
			this.Parameters.ForEach(p => m.Parameters.Add(p.Clone(m)));
			m.MaximumStackDepth = this.MaximumStackDepth;
			m.Name = this.Name;
			m.ParentType = newParent;
			m.ReturnType = this.ReturnType;
			m.IsStatic = this.IsStatic;
			// TODO: Fix Branch/Switch/Leave IRInstruction's to new method instructions based on IRIndex's
			return m;
		}

        // Dynamic Methods
        public bool IsGeneric
        {
            get { return GenericParameters.Count > 0; }
        }
        public IRMethod GenericMethod = null;
        public readonly IRGenericParameterList GenericParameters = new IRGenericParameterList();

        // Temporary
        public ushort MaximumStackDepth = 0;
		public IRControlFlowGraph ControlFlowGraph = null;

        public IRMethod(IRAssembly pAssembly)
        {
            Assembly = pAssembly;
			Assembly.AppDomain.Methods.Add(this);
        }

		public override string ToString()
		{
			StringBuilder sb = new StringBuilder();

			sb.Append(ReturnType == null ? "(null)" : ReturnType.ToString());
			sb.Append(" ");
			sb.Append(ParentType.ToString());
			sb.Append(IsStatic ? "::" : ".");
			sb.Append(Name);
			if (IsGeneric)
				sb.Append(GenericParameters.ToString());
			sb.Append("(");
			Parameters.ForEach(p => sb.Append(p.ToString() + ", "));
			if (Parameters.Count > 0)
				sb.Remove(sb.Length - 2, 2);
			sb.Append(")");

			return sb.ToString();
		}

        public bool CompareSignature(MethodSig pMethodSig)
        {
            bool implicitThis = pMethodSig.HasThis && !pMethodSig.ExplicitThis;
            if ((Parameters.Count - (implicitThis ? 1 : 0)) != pMethodSig.Params.Count) return false;
            if (ReturnType != Assembly.AppDomain.PresolveType(pMethodSig.RetType)) return false;
            for (int index = 0; index < pMethodSig.Params.Count; ++index)
            {
                if (Parameters[index + (implicitThis ? 1 : 0)].Type != Assembly.AppDomain.PresolveType(pMethodSig.Params[index])) return false;
            }
            return true;
        }

        public bool CompareSignature(MemberRefData pMemberRefData)
        {
            if (Name != pMemberRefData.Name) return false;
            return CompareSignature(pMemberRefData.ExpandedMethodSignature);
        }

        public void AddInstruction(uint pILOffset, IRInstruction pInstruction)
        {
            pInstruction.ILOffset = (int)pILOffset;
            pInstruction.IRIndex = (uint)Instructions.Count;
            pInstruction.ParentMethod = this;
            Instructions.Add(pInstruction);
        }

        public void ConvertInstructions(MethodDefData pMethodDefData)
        {
            if (pMethodDefData.Body == null) return;

            ILReader reader = new ILReader(pMethodDefData.CLIFile.Data, pMethodDefData.Body.CodeRVA, pMethodDefData.Body.CodeSize);
            ILOpcode opcode = ILOpcode.Nop;
            ILExtendedOpcode extendedOpcode = ILExtendedOpcode.ArgList;
            MethodSig methodSignature = pMethodDefData.ExpandedSignature;
            IRPrefixFlags prefixFlags = IRPrefixFlags.None;
            uint prefixConstrainedToken = 0;

            Console.WriteLine("Converting {0}.{1}.{2}", ParentType.Namespace, ParentType.Name, Name);
            while (!reader.EndOfCode)
            {
                bool clearFlags = true;
                uint startOfInstruction = reader.Offset;
                opcode = reader.ReadOpcode();
                switch (opcode)
                {
                    case ILOpcode.Nop: AddInstruction(startOfInstruction, new IRNopInstruction(true)); break;
                    case ILOpcode.Break: AddInstruction(startOfInstruction, new IRBreakInstruction()); break;
                    case ILOpcode.LdArg_0: AddInstruction(startOfInstruction, new IRLoadParameterInstruction(0)); break;
                    case ILOpcode.LdArg_1: AddInstruction(startOfInstruction, new IRLoadParameterInstruction(1)); break;
                    case ILOpcode.LdArg_2: AddInstruction(startOfInstruction, new IRLoadParameterInstruction(2)); break;
                    case ILOpcode.LdArg_3: AddInstruction(startOfInstruction, new IRLoadParameterInstruction(3)); break;
                    case ILOpcode.LdLoc_0: AddInstruction(startOfInstruction, new IRLoadLocalInstruction(0)); break;
                    case ILOpcode.LdLoc_1: AddInstruction(startOfInstruction, new IRLoadLocalInstruction(1)); break;
                    case ILOpcode.LdLoc_2: AddInstruction(startOfInstruction, new IRLoadLocalInstruction(2)); break;
                    case ILOpcode.LdLoc_3: AddInstruction(startOfInstruction, new IRLoadLocalInstruction(3)); break;
                    case ILOpcode.StLoc_0: AddInstruction(startOfInstruction, new IRStoreLocalInstruction(0)); break;
                    case ILOpcode.StLoc_1: AddInstruction(startOfInstruction, new IRStoreLocalInstruction(1)); break;
                    case ILOpcode.StLoc_2: AddInstruction(startOfInstruction, new IRStoreLocalInstruction(2)); break;
                    case ILOpcode.StLoc_3: AddInstruction(startOfInstruction, new IRStoreLocalInstruction(3)); break;
                    case ILOpcode.LdArg_S: AddInstruction(startOfInstruction, new IRLoadParameterInstruction(reader.ReadByte())); break;
                    case ILOpcode.LdArgA_S: AddInstruction(startOfInstruction, new IRLoadParameterAddressInstruction(reader.ReadByte())); break;
                    case ILOpcode.StArg_S: AddInstruction(startOfInstruction, new IRStoreParameterInstruction(reader.ReadByte())); break;
                    case ILOpcode.LdLoc_S: AddInstruction(startOfInstruction, new IRLoadLocalInstruction(reader.ReadByte())); break;
                    case ILOpcode.LdLocA_S: AddInstruction(startOfInstruction, new IRLoadLocalAddressInstruction(reader.ReadByte())); break;
                    case ILOpcode.StLoc_S: AddInstruction(startOfInstruction, new IRStoreLocalInstruction(reader.ReadByte())); break;
                    case ILOpcode.LdNull: AddInstruction(startOfInstruction, new IRLoadNullInstruction()); break;
                    case ILOpcode.Ldc_I4_M1: AddInstruction(startOfInstruction, new IRLoadInteger32Instruction(-1)); break;
                    case ILOpcode.Ldc_I4_0: AddInstruction(startOfInstruction, new IRLoadInteger32Instruction(0)); break;
                    case ILOpcode.Ldc_I4_1: AddInstruction(startOfInstruction, new IRLoadInteger32Instruction(1)); break;
                    case ILOpcode.Ldc_I4_2: AddInstruction(startOfInstruction, new IRLoadInteger32Instruction(2)); break;
                    case ILOpcode.Ldc_I4_3: AddInstruction(startOfInstruction, new IRLoadInteger32Instruction(3)); break;
                    case ILOpcode.Ldc_I4_4: AddInstruction(startOfInstruction, new IRLoadInteger32Instruction(4)); break;
                    case ILOpcode.Ldc_I4_5: AddInstruction(startOfInstruction, new IRLoadInteger32Instruction(5)); break;
                    case ILOpcode.Ldc_I4_6: AddInstruction(startOfInstruction, new IRLoadInteger32Instruction(6)); break;
                    case ILOpcode.Ldc_I4_7: AddInstruction(startOfInstruction, new IRLoadInteger32Instruction(7)); break;
                    case ILOpcode.Ldc_I4_8: AddInstruction(startOfInstruction, new IRLoadInteger32Instruction(8)); break;
                    case ILOpcode.Ldc_I4_S: AddInstruction(startOfInstruction, new IRLoadInteger32Instruction(reader.ReadByte())); break;
                    case ILOpcode.Ldc_I4: AddInstruction(startOfInstruction, new IRLoadInteger32Instruction(reader.ReadInt32())); break;
                    case ILOpcode.Ldc_I8: AddInstruction(startOfInstruction, new IRLoadInteger64Instruction(reader.ReadInt64())); break;
                    case ILOpcode.Ldc_R4: AddInstruction(startOfInstruction, new IRLoadReal32Instruction(reader.ReadSingle())); break;
                    case ILOpcode.Ldc_R8: AddInstruction(startOfInstruction, new IRLoadReal64Instruction(reader.ReadDouble())); break;
                    case ILOpcode.Dup: AddInstruction(startOfInstruction, new IRDuplicateInstruction()); break;
                    case ILOpcode.Pop: AddInstruction(startOfInstruction, new IRPopInstruction()); break;
                    case ILOpcode.Jmp: AddInstruction(startOfInstruction, new IRJumpInstruction(Assembly.AppDomain.PresolveMethod(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())))); break;
                    case ILOpcode.Call: AddInstruction(startOfInstruction, new IRCallInstruction(Assembly.AppDomain.PresolveMethod(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())), false)); break;
                    case ILOpcode.CallI: throw new NotImplementedException("CallI");
                    case ILOpcode.Ret: AddInstruction(startOfInstruction, new IRReturnInstruction()); break;
                    case ILOpcode.Br_S: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.Always, (int)(reader.ReadSByte() + reader.Offset))); break;
                    case ILOpcode.BrFalse_S: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.False, (int)(reader.ReadSByte() + reader.Offset))); break;
                    case ILOpcode.BrTrue_S: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.True, (int)(reader.ReadSByte() + reader.Offset))); break;
                    case ILOpcode.Beq_S: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.Equal, (int)(reader.ReadSByte() + reader.Offset))); break;
                    case ILOpcode.Bge_S: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.GreaterOrEqual, (int)(reader.ReadSByte() + reader.Offset))); break;
                    case ILOpcode.Bgt_S: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.Greater, (int)(reader.ReadSByte() + reader.Offset))); break;
                    case ILOpcode.Ble_S: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.LessOrEqual, (int)(reader.ReadSByte() + reader.Offset))); break;
                    case ILOpcode.Blt_S: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.Less, (int)(reader.ReadSByte() + reader.Offset))); break;
                    case ILOpcode.Bne_Un_S: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.NotEqualUnsigned, (int)(reader.ReadSByte() + reader.Offset))); break;
                    case ILOpcode.Bge_Un_S: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.GreaterOrEqualUnsigned, (int)(reader.ReadSByte() + reader.Offset))); break;
                    case ILOpcode.Bgt_Un_S: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.GreaterUnsigned, (int)(reader.ReadSByte() + reader.Offset))); break;
                    case ILOpcode.Ble_Un_S: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.LessOrEqualUnsigned, (int)(reader.ReadSByte() + reader.Offset))); break;
                    case ILOpcode.Blt_Un_S: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.LessUnsigned, (int)(reader.ReadSByte() + reader.Offset))); break;
                    case ILOpcode.Br: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.Always, (int)(reader.ReadInt32() + reader.Offset))); break;
                    case ILOpcode.BrFalse: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.False, (int)(reader.ReadInt32() + reader.Offset))); break;
                    case ILOpcode.BrTrue: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.True, (int)(reader.ReadInt32() + reader.Offset))); break;
                    case ILOpcode.Beq: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.Equal, (int)(reader.ReadInt32() + reader.Offset))); break;
                    case ILOpcode.Bge: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.GreaterOrEqual, (int)(reader.ReadInt32() + reader.Offset))); break;
                    case ILOpcode.Bgt: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.Greater, (int)(reader.ReadInt32() + reader.Offset))); break;
                    case ILOpcode.Ble: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.LessOrEqual, (int)(reader.ReadInt32() + reader.Offset))); break;
                    case ILOpcode.Blt: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.Less, (int)(reader.ReadInt32() + reader.Offset))); break;
                    case ILOpcode.Bne_Un: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.NotEqualUnsigned, (int)(reader.ReadInt32() + reader.Offset))); break;
                    case ILOpcode.Bge_Un: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.GreaterOrEqualUnsigned, (int)(reader.ReadInt32() + reader.Offset))); break;
                    case ILOpcode.Bgt_Un: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.GreaterUnsigned, (int)(reader.ReadInt32() + reader.Offset))); break;
                    case ILOpcode.Ble_Un: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.LessOrEqualUnsigned, (int)(reader.ReadInt32() + reader.Offset))); break;
                    case ILOpcode.Blt_Un: AddInstruction(startOfInstruction, new IRBranchInstruction(IRBranchCondition.LessUnsigned, (int)(reader.ReadInt32() + reader.Offset))); break;
                    case ILOpcode.Switch:
                        {
                            uint targetCount = reader.ReadUInt32();
                            int[] targetILOffsets = new int[targetCount];
                            for (int index = 0; index < targetCount; ++index) targetILOffsets[index] = reader.ReadInt32();
                            for (int index = 0; index < targetCount; ++index) targetILOffsets[index] += (int)reader.Offset;
                            AddInstruction(startOfInstruction, new IRSwitchInstruction(targetILOffsets));
                            break;
                        }
                    case ILOpcode.LdInd_I1: AddInstruction(startOfInstruction, new IRLoadIndirectInstruction(Assembly.AppDomain.System_SByte)); break;
                    case ILOpcode.LdInd_U1: AddInstruction(startOfInstruction, new IRLoadIndirectInstruction(Assembly.AppDomain.System_Byte)); break;
                    case ILOpcode.LdInd_I2: AddInstruction(startOfInstruction, new IRLoadIndirectInstruction(Assembly.AppDomain.System_Int16)); break;
                    case ILOpcode.LdInd_U2: AddInstruction(startOfInstruction, new IRLoadIndirectInstruction(Assembly.AppDomain.System_UInt16)); break;
                    case ILOpcode.LdInd_I4: AddInstruction(startOfInstruction, new IRLoadIndirectInstruction(Assembly.AppDomain.System_Int32)); break;
                    case ILOpcode.LdInd_U4: AddInstruction(startOfInstruction, new IRLoadIndirectInstruction(Assembly.AppDomain.System_UInt32)); break;
                    case ILOpcode.LdInd_I8: AddInstruction(startOfInstruction, new IRLoadIndirectInstruction(Assembly.AppDomain.System_Int64)); break;
                    case ILOpcode.LdInd_I: AddInstruction(startOfInstruction, new IRLoadIndirectInstruction(Assembly.AppDomain.System_IntPtr)); break;
                    case ILOpcode.LdInd_R4: AddInstruction(startOfInstruction, new IRLoadIndirectInstruction(Assembly.AppDomain.System_Single)); break;
                    case ILOpcode.LdInd_R8: AddInstruction(startOfInstruction, new IRLoadIndirectInstruction(Assembly.AppDomain.System_Double)); break;
                    case ILOpcode.LdInd_Ref: AddInstruction(startOfInstruction, new IRLoadIndirectInstruction(Assembly.AppDomain.System_Object)); break;
                    case ILOpcode.StInd_Ref: AddInstruction(startOfInstruction, new IRStoreIndirectInstruction(Assembly.AppDomain.System_Object)); break;
                    case ILOpcode.StInd_I1: AddInstruction(startOfInstruction, new IRStoreIndirectInstruction(Assembly.AppDomain.System_SByte)); break;
                    case ILOpcode.StInd_I2: AddInstruction(startOfInstruction, new IRStoreIndirectInstruction(Assembly.AppDomain.System_Int16)); break;
                    case ILOpcode.StInd_I4: AddInstruction(startOfInstruction, new IRStoreIndirectInstruction(Assembly.AppDomain.System_Int32)); break;
                    case ILOpcode.StInd_I8: AddInstruction(startOfInstruction, new IRStoreIndirectInstruction(Assembly.AppDomain.System_Int64)); break;
                    case ILOpcode.StInd_R4: AddInstruction(startOfInstruction, new IRStoreIndirectInstruction(Assembly.AppDomain.System_Single)); break;
                    case ILOpcode.StInd_R8: AddInstruction(startOfInstruction, new IRStoreIndirectInstruction(Assembly.AppDomain.System_Double)); break;
                    case ILOpcode.Add: AddInstruction(startOfInstruction, new IRAddInstruction(IROverflowType.None)); break;
                    case ILOpcode.Sub: AddInstruction(startOfInstruction, new IRSubtractInstruction(IROverflowType.None)); break;
                    case ILOpcode.Mul: AddInstruction(startOfInstruction, new IRMultiplyInstruction(IROverflowType.None)); break;
                    case ILOpcode.Div: AddInstruction(startOfInstruction, new IRDivideInstruction(IROverflowType.Signed)); break;
                    case ILOpcode.Div_Un: AddInstruction(startOfInstruction, new IRDivideInstruction(IROverflowType.Unsigned)); break;
                    case ILOpcode.Rem: AddInstruction(startOfInstruction, new IRRemainderInstruction(IROverflowType.Signed)); break;
                    case ILOpcode.Rem_Un: AddInstruction(startOfInstruction, new IRRemainderInstruction(IROverflowType.Unsigned)); break;
                    case ILOpcode.And: AddInstruction(startOfInstruction, new IRAndInstruction()); break;
                    case ILOpcode.Or: AddInstruction(startOfInstruction, new IROrInstruction()); break;
                    case ILOpcode.Xor: AddInstruction(startOfInstruction, new IRXorInstruction()); break;
                    case ILOpcode.Shl: AddInstruction(startOfInstruction, new IRShiftInstruction(IRShiftType.Left)); break;
                    case ILOpcode.Shr: AddInstruction(startOfInstruction, new IRShiftInstruction(IRShiftType.RightSignExtended)); break;
                    case ILOpcode.Shr_Un: AddInstruction(startOfInstruction, new IRShiftInstruction(IRShiftType.Right)); break;
                    case ILOpcode.Neg: AddInstruction(startOfInstruction, new IRNegateInstruction()); break;
                    case ILOpcode.Not: AddInstruction(startOfInstruction, new IRNotInstruction()); break;
                    case ILOpcode.Conv_I1: AddInstruction(startOfInstruction, new IRConvertUncheckedInstruction(Assembly.AppDomain.System_SByte)); break;
                    case ILOpcode.Conv_I2: AddInstruction(startOfInstruction, new IRConvertUncheckedInstruction(Assembly.AppDomain.System_Int16)); break;
                    case ILOpcode.Conv_I4: AddInstruction(startOfInstruction, new IRConvertUncheckedInstruction(Assembly.AppDomain.System_Int32)); break;
                    case ILOpcode.Conv_I8: AddInstruction(startOfInstruction, new IRConvertUncheckedInstruction(Assembly.AppDomain.System_Int64)); break;
                    case ILOpcode.Conv_R4: AddInstruction(startOfInstruction, new IRConvertUncheckedInstruction(Assembly.AppDomain.System_Single)); break;
                    case ILOpcode.Conv_R8: AddInstruction(startOfInstruction, new IRConvertUncheckedInstruction(Assembly.AppDomain.System_Double)); break;
                    case ILOpcode.Conv_U4: AddInstruction(startOfInstruction, new IRConvertUncheckedInstruction(Assembly.AppDomain.System_UInt32)); break;
                    case ILOpcode.Conv_U8: AddInstruction(startOfInstruction, new IRConvertUncheckedInstruction(Assembly.AppDomain.System_UInt64)); break;
                    case ILOpcode.CallVirt: AddInstruction(startOfInstruction, new IRCallInstruction(Assembly.AppDomain.PresolveMethod(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())), true)); break;

                    case ILOpcode.CpObj: AddInstruction(startOfInstruction, new IRCopyObjectInstruction(Assembly.AppDomain.PresolveType(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())))); break;
                    case ILOpcode.LdObj: AddInstruction(startOfInstruction, new IRLoadObjectInstruction(Assembly.AppDomain.PresolveType(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())))); break;
                    case ILOpcode.LdStr: AddInstruction(startOfInstruction, new IRLoadStringInstruction((string)Assembly.File.ExpandMetadataToken(reader.ReadUInt32()).Data)); break;
                    case ILOpcode.NewObj: AddInstruction(startOfInstruction, new IRNewObjectInstruction(Assembly.AppDomain.PresolveMethod(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())))); break;
                    case ILOpcode.CastClass: AddInstruction(startOfInstruction, new IRCastInstruction(Assembly.AppDomain.PresolveType(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())), true)); break;
                    case ILOpcode.IsInst: AddInstruction(startOfInstruction, new IRCastInstruction(Assembly.AppDomain.PresolveType(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())), false)); break;

                    case ILOpcode.Conv_R_Un: throw new NotImplementedException("Conv_R_Un");
                    case ILOpcode.Unbox: AddInstruction(startOfInstruction, new IRUnboxInstruction(Assembly.AppDomain.PresolveType(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())), false)); break;
                    case ILOpcode.Throw: AddInstruction(startOfInstruction, new IRThrowInstruction()); break;
                    case ILOpcode.LdFld: AddInstruction(startOfInstruction, new IRLoadFieldInstruction(Assembly.AppDomain.PresolveField(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())))); break;
                    case ILOpcode.LdFldA: AddInstruction(startOfInstruction, new IRLoadFieldAddressInstruction(Assembly.AppDomain.PresolveField(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())))); break;
                    case ILOpcode.StFld: AddInstruction(startOfInstruction, new IRStoreFieldInstruction(Assembly.AppDomain.PresolveField(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())))); break;
                    case ILOpcode.LdSFld: AddInstruction(startOfInstruction, new IRLoadStaticFieldInstruction(Assembly.AppDomain.PresolveField(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())))); break;
                    case ILOpcode.LdSFldA: AddInstruction(startOfInstruction, new IRLoadStaticFieldAddressInstruction(Assembly.AppDomain.PresolveField(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())))); break;
                    case ILOpcode.StSFld: AddInstruction(startOfInstruction, new IRStoreStaticFieldInstruction(Assembly.AppDomain.PresolveField(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())))); break;
                    case ILOpcode.StObj: AddInstruction(startOfInstruction, new IRStoreObjectInstruction(Assembly.AppDomain.PresolveType(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())))); break;
                    case ILOpcode.Conv_Ovf_I1_Un: AddInstruction(startOfInstruction, new IRConvertCheckedInstruction(Assembly.AppDomain.System_SByte, IROverflowType.Unsigned)); break;
                    case ILOpcode.Conv_Ovf_I2_Un: AddInstruction(startOfInstruction, new IRConvertCheckedInstruction(Assembly.AppDomain.System_Int16, IROverflowType.Unsigned)); break;
                    case ILOpcode.Conv_Ovf_I4_Un: AddInstruction(startOfInstruction, new IRConvertCheckedInstruction(Assembly.AppDomain.System_Int32, IROverflowType.Unsigned)); break;
                    case ILOpcode.Conv_Ovf_I8_Un: AddInstruction(startOfInstruction, new IRConvertCheckedInstruction(Assembly.AppDomain.System_Int64, IROverflowType.Unsigned)); break;
                    case ILOpcode.Conv_Ovf_U1_Un: AddInstruction(startOfInstruction, new IRConvertCheckedInstruction(Assembly.AppDomain.System_SByte, IROverflowType.Unsigned)); break;
                    case ILOpcode.Conv_Ovf_U2_Un: AddInstruction(startOfInstruction, new IRConvertCheckedInstruction(Assembly.AppDomain.System_Int16, IROverflowType.Unsigned)); break;
                    case ILOpcode.Conv_Ovf_U4_Un: AddInstruction(startOfInstruction, new IRConvertCheckedInstruction(Assembly.AppDomain.System_Int32, IROverflowType.Unsigned)); break;
                    case ILOpcode.Conv_Ovf_U8_Un: AddInstruction(startOfInstruction, new IRConvertCheckedInstruction(Assembly.AppDomain.System_Int64, IROverflowType.Unsigned)); break;
                    case ILOpcode.Conv_Ovf_I_Un: AddInstruction(startOfInstruction, new IRConvertCheckedInstruction(Assembly.AppDomain.System_IntPtr, IROverflowType.Unsigned)); break;
                    case ILOpcode.Conv_Ovf_U_Un: AddInstruction(startOfInstruction, new IRConvertCheckedInstruction(Assembly.AppDomain.System_UIntPtr, IROverflowType.Unsigned)); break;

                    case ILOpcode.Box: AddInstruction(startOfInstruction, new IRBoxInstruction(Assembly.AppDomain.PresolveType(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())))); break;
                    case ILOpcode.NewArr: AddInstruction(startOfInstruction, new IRNewArrayInstruction(Assembly.AppDomain.PresolveType(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())))); break;
                    case ILOpcode.LdLen: AddInstruction(startOfInstruction, new IRLoadArrayLengthInstruction()); break;
                    case ILOpcode.LdElemA: AddInstruction(startOfInstruction, new IRLoadArrayElementAddressInstruction(Assembly.AppDomain.PresolveType(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())))); break;
                    case ILOpcode.LdElem_I1: AddInstruction(startOfInstruction, new IRLoadArrayElementInstruction(Assembly.AppDomain.System_SByte)); break;
                    case ILOpcode.LdElem_U1: AddInstruction(startOfInstruction, new IRLoadArrayElementInstruction(Assembly.AppDomain.System_Byte)); break;
                    case ILOpcode.LdElem_I2: AddInstruction(startOfInstruction, new IRLoadArrayElementInstruction(Assembly.AppDomain.System_Int16)); break;
                    case ILOpcode.LdElem_U2: AddInstruction(startOfInstruction, new IRLoadArrayElementInstruction(Assembly.AppDomain.System_UInt16)); break;
                    case ILOpcode.LdElem_I4: AddInstruction(startOfInstruction, new IRLoadArrayElementInstruction(Assembly.AppDomain.System_Int32)); break;
                    case ILOpcode.LdElem_U4: AddInstruction(startOfInstruction, new IRLoadArrayElementInstruction(Assembly.AppDomain.System_UInt32)); break;
                    case ILOpcode.LdElem_I8: AddInstruction(startOfInstruction, new IRLoadArrayElementInstruction(Assembly.AppDomain.System_Int64)); break;
                    case ILOpcode.LdElem_I: AddInstruction(startOfInstruction, new IRLoadArrayElementInstruction(Assembly.AppDomain.System_IntPtr)); break;
                    case ILOpcode.LdElem_R4: AddInstruction(startOfInstruction, new IRLoadArrayElementInstruction(Assembly.AppDomain.System_Single)); break;
                    case ILOpcode.LdElem_R8: AddInstruction(startOfInstruction, new IRLoadArrayElementInstruction(Assembly.AppDomain.System_Double)); break;
                    case ILOpcode.LdElem_Ref: AddInstruction(startOfInstruction, new IRLoadArrayElementInstruction(null)); break;
                    case ILOpcode.StElem_I: AddInstruction(startOfInstruction, new IRStoreArrayElementInstruction(Assembly.AppDomain.System_IntPtr)); break;
                    case ILOpcode.StElem_I1: AddInstruction(startOfInstruction, new IRStoreArrayElementInstruction(Assembly.AppDomain.System_SByte)); break;
                    case ILOpcode.StElem_I2: AddInstruction(startOfInstruction, new IRStoreArrayElementInstruction(Assembly.AppDomain.System_Int16)); break;
                    case ILOpcode.StElem_I4: AddInstruction(startOfInstruction, new IRStoreArrayElementInstruction(Assembly.AppDomain.System_Int32)); break;
                    case ILOpcode.StElem_I8: AddInstruction(startOfInstruction, new IRStoreArrayElementInstruction(Assembly.AppDomain.System_Int64)); break;
                    case ILOpcode.StElem_R4: AddInstruction(startOfInstruction, new IRStoreArrayElementInstruction(Assembly.AppDomain.System_Single)); break;
                    case ILOpcode.StElem_R8: AddInstruction(startOfInstruction, new IRStoreArrayElementInstruction(Assembly.AppDomain.System_Double)); break;
                    case ILOpcode.StElem_Ref: AddInstruction(startOfInstruction, new IRStoreArrayElementInstruction(null)); break;
                    case ILOpcode.LdElem: AddInstruction(startOfInstruction, new IRLoadArrayElementInstruction(Assembly.AppDomain.PresolveType(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())))); break;
                    case ILOpcode.StElem: AddInstruction(startOfInstruction, new IRStoreArrayElementInstruction(Assembly.AppDomain.PresolveType(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())))); break;
                    case ILOpcode.Unbox_Any: AddInstruction(startOfInstruction, new IRUnboxInstruction(Assembly.AppDomain.PresolveType(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())), true)); break;
                    case ILOpcode.Conv_Ovf_I1: AddInstruction(startOfInstruction, new IRConvertCheckedInstruction(Assembly.AppDomain.System_SByte, IROverflowType.Signed)); break;
                    case ILOpcode.Conv_Ovf_U1: AddInstruction(startOfInstruction, new IRConvertCheckedInstruction(Assembly.AppDomain.System_Byte, IROverflowType.Signed)); break;
                    case ILOpcode.Conv_Ovf_I2: AddInstruction(startOfInstruction, new IRConvertCheckedInstruction(Assembly.AppDomain.System_Int16, IROverflowType.Signed)); break;
                    case ILOpcode.Conv_Ovf_U2: AddInstruction(startOfInstruction, new IRConvertCheckedInstruction(Assembly.AppDomain.System_UInt16, IROverflowType.Signed)); break;
                    case ILOpcode.Conv_Ovf_I4: AddInstruction(startOfInstruction, new IRConvertCheckedInstruction(Assembly.AppDomain.System_Int32, IROverflowType.Signed)); break;
                    case ILOpcode.Conv_Ovf_U4: AddInstruction(startOfInstruction, new IRConvertCheckedInstruction(Assembly.AppDomain.System_UInt32, IROverflowType.Signed)); break;
                    case ILOpcode.Conv_Ovf_I8: AddInstruction(startOfInstruction, new IRConvertCheckedInstruction(Assembly.AppDomain.System_Int64, IROverflowType.Signed)); break;
                    case ILOpcode.Conv_Ovf_U8: AddInstruction(startOfInstruction, new IRConvertCheckedInstruction(Assembly.AppDomain.System_UInt64, IROverflowType.Signed)); break;
                    case ILOpcode.RefAnyVal: AddInstruction(startOfInstruction, new IRLoadTypedReferenceAddressInstruction(Assembly.AppDomain.PresolveType(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())))); break;
                    case ILOpcode.CkFinite: AddInstruction(startOfInstruction, new IRCheckFiniteInstruction()); break;
                    case ILOpcode.MkRefAny: AddInstruction(startOfInstruction, new IRLoadTypedReferenceInstruction(Assembly.AppDomain.PresolveType(Assembly.File.ExpandTypeDefRefOrSpecToken(reader.ReadUInt32())))); break;
                    case ILOpcode.LdToken:
                        {
                            IRType type = null;
                            IRMethod method = null;
                            IRField field = null;
                            MetadataToken token = Assembly.File.ExpandMetadataToken(reader.ReadUInt32());
                            switch (token.Table)
                            {
                                case MetadataTables.TypeDef: type = Assembly.AppDomain.PresolveType((TypeDefData)token.Data); break;
                                case MetadataTables.TypeRef: type = Assembly.AppDomain.PresolveType((TypeRefData)token.Data); break;
                                case MetadataTables.TypeSpec: type = Assembly.AppDomain.PresolveType((TypeSpecData)token.Data); break;
                                case MetadataTables.MethodDef: method = Assembly.AppDomain.PresolveMethod((MethodDefData)token.Data); break;
                                case MetadataTables.MethodSpec: method = Assembly.AppDomain.PresolveMethod((MethodSpecData)token.Data); break;
                                case MetadataTables.Field: field = Assembly.AppDomain.PresolveField((FieldData)token.Data); break;
                                case MetadataTables.MemberRef:
                                    {
                                        MemberRefData memberRefData = (MemberRefData)token.Data;
                                        if (memberRefData.IsMethodRef) method = Assembly.AppDomain.PresolveMethod(memberRefData);
                                        else field = Assembly.AppDomain.PresolveField(memberRefData);
                                        break;
                                    }
                            }
                            AddInstruction(startOfInstruction, new IRLoadRuntimeHandleInstruction(type, method, field));
                            break;
                        }
                    case ILOpcode.Conv_U2: AddInstruction(startOfInstruction, new IRConvertUncheckedInstruction(Assembly.AppDomain.System_UInt16)); break;
                    case ILOpcode.Conv_U1: AddInstruction(startOfInstruction, new IRConvertUncheckedInstruction(Assembly.AppDomain.System_Byte)); break;
                    case ILOpcode.Conv_I: AddInstruction(startOfInstruction, new IRConvertUncheckedInstruction(Assembly.AppDomain.System_IntPtr)); break;
                    case ILOpcode.Conv_Ovf_I: AddInstruction(startOfInstruction, new IRConvertCheckedInstruction(Assembly.AppDomain.System_IntPtr, IROverflowType.Signed)); break;
                    case ILOpcode.Conv_Ovf_U: AddInstruction(startOfInstruction, new IRConvertCheckedInstruction(Assembly.AppDomain.System_UIntPtr, IROverflowType.Signed)); break;
                    case ILOpcode.Add_Ovf: AddInstruction(startOfInstruction, new IRAddInstruction(IROverflowType.Signed)); break;
                    case ILOpcode.Add_Ovf_Un: AddInstruction(startOfInstruction, new IRAddInstruction(IROverflowType.Unsigned)); break;
                    case ILOpcode.Mul_Ovf: AddInstruction(startOfInstruction, new IRMultiplyInstruction(IROverflowType.Signed)); break;
                    case ILOpcode.Mul_Ovf_Un: AddInstruction(startOfInstruction, new IRMultiplyInstruction(IROverflowType.Unsigned)); break;
                    case ILOpcode.Sub_Ovf: AddInstruction(startOfInstruction, new IRSubtractInstruction(IROverflowType.Signed)); break;
                    case ILOpcode.Sub_Ovf_Un: AddInstruction(startOfInstruction, new IRSubtractInstruction(IROverflowType.Unsigned)); break;
                    case ILOpcode.EndFinally: AddInstruction(startOfInstruction, new IREndFinallyInstruction()); break;
                    case ILOpcode.Leave: AddInstruction(startOfInstruction, new IRLeaveInstruction((int)(reader.ReadInt32() + reader.Offset))); break;
                    case ILOpcode.Leave_S: AddInstruction(startOfInstruction, new IRLeaveInstruction((int)(reader.ReadByte() + reader.Offset))); break;
                    case ILOpcode.StInd_I: AddInstruction(startOfInstruction, new IRStoreIndirectInstruction(Assembly.AppDomain.System_IntPtr)); break;
                    case ILOpcode.Conv_U: AddInstruction(startOfInstruction, new IRConvertUncheckedInstruction(Assembly.AppDomain.System_UIntPtr)); break;

                    case ILOpcode.Extended:
                        {
                            extendedOpcode = (ILExtendedOpcode)reader.ReadByte();
                            switch (extendedOpcode)
                            {
                                case ILExtendedOpcode.ArgList: throw new NotImplementedException("ArgList");
                                case ILExtendedOpcode.Ceq: AddInstruction(startOfInstruction, new IRCompareInstruction(IRCompareCondition.Equal)); break;
                                case ILExtendedOpcode.Cgt: AddInstruction(startOfInstruction, new IRCompareInstruction(IRCompareCondition.GreaterThan)); break;
                                case ILExtendedOpcode.Cgt_Un: AddInstruction(startOfInstruction, new IRCompareInstruction(IRCompareCondition.GreaterThanUnsigned)); break;
                                case ILExtendedOpcode.Clt: AddInstruction(startOfInstruction, new IRCompareInstruction(IRCompareCondition.LessThan)); break;
                                case ILExtendedOpcode.Clt_Un: AddInstruction(startOfInstruction, new IRCompareInstruction(IRCompareCondition.LessThanUnsigned)); break;
                                case ILExtendedOpcode.LdFtn: AddInstruction(startOfInstruction, new IRLoadFunctionInstruction(Assembly.AppDomain.PresolveMethod(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())), false)); break;
                                case ILExtendedOpcode.LdVirtFtn: AddInstruction(startOfInstruction, new IRLoadFunctionInstruction(Assembly.AppDomain.PresolveMethod(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())), true)); break;
                                case ILExtendedOpcode.LdArg: AddInstruction(startOfInstruction, new IRLoadParameterInstruction(reader.ReadUInt16())); break;
                                case ILExtendedOpcode.LdArgA: AddInstruction(startOfInstruction, new IRLoadParameterAddressInstruction(reader.ReadUInt16())); break;
                                case ILExtendedOpcode.StArg: AddInstruction(startOfInstruction, new IRStoreParameterInstruction(reader.ReadUInt16())); break;
                                case ILExtendedOpcode.LdLoc: AddInstruction(startOfInstruction, new IRLoadLocalInstruction(reader.ReadUInt16())); break;
                                case ILExtendedOpcode.LdLocA: AddInstruction(startOfInstruction, new IRLoadLocalAddressInstruction(reader.ReadUInt16())); break;
                                case ILExtendedOpcode.StLoc: AddInstruction(startOfInstruction, new IRStoreLocalInstruction(reader.ReadUInt16())); break;
                                case ILExtendedOpcode.LocAlloc: AddInstruction(startOfInstruction, new IRStackAllocateInstruction()); break;
                                case ILExtendedOpcode.EndFilter: throw new NotImplementedException("EndFilter");
                                case ILExtendedOpcode.Unaligned__: prefixFlags |= IRPrefixFlags.Unaligned; clearFlags = false; break;
                                case ILExtendedOpcode.Volatile__: prefixFlags |= IRPrefixFlags.Volatile; clearFlags = false; break;
                                case ILExtendedOpcode.Tail__: prefixFlags |= IRPrefixFlags.Tail; clearFlags = false; break;
                                case ILExtendedOpcode.InitObj: AddInstruction(startOfInstruction, new IRInitializeObjectInstruction(Assembly.AppDomain.PresolveType(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())))); break;
                                case ILExtendedOpcode.Constrained__: prefixConstrainedToken = reader.ReadUInt32(); prefixFlags |= IRPrefixFlags.Constrained; clearFlags = false; break;
                                case ILExtendedOpcode.CpBlk: AddInstruction(startOfInstruction, new IRCopyBlockInstruction()); break;
                                case ILExtendedOpcode.InitBlk: AddInstruction(startOfInstruction, new IRInitializeBlockInstruction()); break;
                                case ILExtendedOpcode.No__: prefixFlags |= IRPrefixFlags.No; clearFlags = false; break;
                                case ILExtendedOpcode.ReThrow: throw new NotImplementedException("ReThrow");
                                case ILExtendedOpcode.SizeOf: AddInstruction(startOfInstruction, new IRSizeOfInstruction(Assembly.AppDomain.PresolveType(Assembly.File.ExpandMetadataToken(reader.ReadUInt32())))); break;
                                case ILExtendedOpcode.RefAnyType: AddInstruction(startOfInstruction, new IRLoadTypedReferenceTypeInstruction()); break;
                                case ILExtendedOpcode.ReadOnly__: prefixFlags |= IRPrefixFlags.ReadOnly; clearFlags = false; break;
                            }
                            break;
                        }
                    default: break;
                }
                if (clearFlags)
                {
                    prefixFlags = IRPrefixFlags.None;
                    prefixConstrainedToken = 0;
                }
            }

            Instructions.FixTargetInstructions();
        }

        private Dictionary<Tuple<int, IRType>, IRLocal> LinearizedStackLocalLookup = new Dictionary<Tuple<int, IRType>, IRLocal>();

        public int AddLinearizedLocal(Stack<IRStackObject> pStack, IRType pType)
        {
            IRLocal local = null;
            if (!LinearizedStackLocalLookup.TryGetValue(new Tuple<int, IRType>(pStack.Count, pType), out local))
            {
                local = new IRLocal(Assembly);
                local.ParentMethod = this;
                local.Type = pType;
                local.Index = Locals.Count;
                Locals.Add(local);
                LinearizedStackLocalLookup[new Tuple<int, IRType>(pStack.Count, pType)] = local;
            }
            return local.Index;
        }

        private void LinearizePath(MethodDefData pMethodDefData, IRInstruction pStartInstruction, Stack<IRStackObject> pStack, Queue<Tuple<IRInstruction, Stack<IRStackObject>>> pBranches)
        {
            int stackReturn = pStack.Count;
            IRInstruction currentInstruction = pStartInstruction;
            MethodDefData.MethodDefBodyData.MethodDefBodyExceptionData exceptionData = null;
            while (currentInstruction != null)
            {
                if (currentInstruction.Linearized && pStack.Count == stackReturn) break;

                if ((exceptionData = Array.Find(pMethodDefData.Body.Exceptions, e => e.Flags == 0 && e.HandlerOffset == currentInstruction.ILOffset)) != null)
                {
                    IRType exceptionType = Assembly.AppDomain.PresolveType(Assembly.File.ExpandMetadataToken(exceptionData.ClassTokenOrFilterOffset));
                    IRStackObject exceptionObj = new IRStackObject();
                    exceptionObj.Type = exceptionType;
                    exceptionObj.LinearizedTarget = new IRLinearizedLocation(IRLinearizedLocationType.Local);
                    exceptionObj.LinearizedTarget.Local.LocalIndex = currentInstruction.AddLinearizedLocal(pStack, exceptionType);
                    pStack.Push(exceptionObj);
                }

                currentInstruction.Linearize(pStack);
                currentInstruction.Linearized = true;
                switch (currentInstruction.Opcode)
                {
                    case IROpcode.Branch:
                        {
                            IRBranchInstruction branchInstruction = (IRBranchInstruction)currentInstruction;
                            if (branchInstruction.BranchCondition == IRBranchCondition.Always) currentInstruction = branchInstruction.TargetIRInstruction;
                            else
                            {
                                pBranches.Enqueue(new Tuple<IRInstruction, Stack<IRStackObject>>(branchInstruction.TargetIRInstruction, pStack.Duplicate()));
                                currentInstruction = Instructions[currentInstruction.IRIndex + 1];
                            }
                            break;
                        }
                    case IROpcode.Switch:
                        {
                            IRSwitchInstruction switchInstruction = (IRSwitchInstruction)currentInstruction;
                            foreach (IRInstruction targetInstruction in switchInstruction.TargetIRInstructions)
                            {
                                pBranches.Enqueue(new Tuple<IRInstruction, Stack<IRStackObject>>(targetInstruction, pStack.Duplicate()));
                            }
                            currentInstruction = Instructions[currentInstruction.IRIndex + 1];
                            break;
                        }
                    case IROpcode.Leave:
                        {
                            IRLeaveInstruction leaveInstruction = (IRLeaveInstruction)currentInstruction;
                            currentInstruction = leaveInstruction.TargetIRInstruction;
                            break;
                        }
                    case IROpcode.Jump:
                    case IROpcode.Throw:
                    case IROpcode.Return: currentInstruction = null; break;
                    default: currentInstruction = currentInstruction.IRIndex >= Instructions.Count ? null : Instructions[currentInstruction.IRIndex + 1]; break;
                }
            }
        }

        public void LinearizeInstructions(MethodDefData pMethodDefData)
        {
            if (Instructions.Count > 0)
            {
                Stack<IRStackObject> stack = new Stack<IRStackObject>((int)MaximumStackDepth);
                Queue<Tuple<IRInstruction, Stack<IRStackObject>>> branches = new Queue<Tuple<IRInstruction, Stack<IRStackObject>>>();
                LinearizePath(pMethodDefData, Instructions[0], stack, branches);
                if (stack.Count > 0) throw new Exception();
                while (branches.Count > 0)
                {
                    Tuple<IRInstruction, Stack<IRStackObject>> branch = branches.Dequeue();
                    int expectedOnStack = branch.Item2.Count;
                    LinearizePath(pMethodDefData, branch.Item1, branch.Item2, branches);
                    if (branch.Item2.Count != expectedOnStack) throw new Exception();
                }
                //if (!Instructions.TrueForAll(i => i.Linearized)) throw new Exception();
            }
        }

        public void TransformInstructions(MethodDefData pMethodDefData)
        {
            for (int index = 0; index < Instructions.Count; ++index) Instructions[index] = Instructions[index].Transform();
        }

		public void EnterSSA()
		{
			if (ControlFlowGraph == null) return;
			int originalCount = Locals.Count;
			bool[] originalAssignments = new bool[originalCount];
			int[] originalIterations = new int[originalCount];
			IRLocal tempLocal = null;

			Locals.ForEach(l => l.SSAData = new IRLocal.IRLocalSSAData(l));
			// Add new local iterations for each assignment to an original local, and keep
			// track of final iterations for each node, assigning true to indicate it was
			// assigned, false means propagated (used later)
			foreach (IRControlFlowGraphNode node in ControlFlowGraph.Nodes)
			{
				node.SSAFinalIterations = new Tuple<IRLocal, bool>[originalCount];
				node.SSAPhis = new IRLocal[originalCount];
				foreach (IRInstruction instruction in node.Instructions)
				{
					if (instruction.Destination == null || instruction.Destination.Type != IRLinearizedLocationType.Local) continue;
					tempLocal = Locals[instruction.Destination.Local.LocalIndex];
					if (!originalAssignments[tempLocal.Index])
					{
						originalAssignments[tempLocal.Index] = true;
						node.SSAFinalIterations[tempLocal.Index] = new Tuple<IRLocal, bool>(tempLocal, true);
						continue;
					}
					tempLocal = tempLocal.Clone(this);
					Locals.Add(tempLocal);
					tempLocal.SSAData.Iteration = ++originalIterations[tempLocal.SSAData.Original.Index];
					instruction.Destination.Local.LocalIndex = tempLocal.Index;
					node.SSAFinalIterations[tempLocal.SSAData.Original.Index] = new Tuple<IRLocal, bool>(tempLocal, true);
				}
			}

			// Any SSAFinalIterations missing from the entry node means the entry node
			// did not assign to the original local, so they can be filled in with
			// propagated original locals by, assigning false to indicate propagated
			for (int index = 0; index < originalCount; ++index)
			{
				if (ControlFlowGraph.Nodes[0].SSAFinalIterations[index] == null)
					ControlFlowGraph.Nodes[0].SSAFinalIterations[index] = new Tuple<IRLocal, bool>(Locals[index], false);
			}
			// Any SSAFinalIterations missing from any node means the node did not
			// assign to the original local, so they can be filled in with propagated
			// locals using the dominance tree, assigning false to indicate propagated
			foreach (IRControlFlowGraphNode node in ControlFlowGraph.Nodes)
			{
				for (int index = 0; index < originalCount; ++index)
				{
					if (node.SSAFinalIterations[index] == null)
					{
						IRControlFlowGraphNode treeNode = node.Dominator;
						while (treeNode.SSAFinalIterations[index] == null) treeNode = treeNode.Dominator;
						node.SSAFinalIterations[index] = new Tuple<IRLocal, bool>(treeNode.SSAFinalIterations[index].Item1, false);
					}
				}
			}

			// Now that all final iterations are known, we also know if the final
			// iteration for a node was assigned or propagated
			// So now we can create a phi, in the dominance frontiers of nodes which
			// have assignments to original locals
			// If the phi is the only assignment in a dominance frontier node, then
			// the phi destination becomes the final iteration for that node
			int localsBeforePhis = Locals.Count;
			BitArray phiInserted = new BitArray(ControlFlowGraph.Nodes.Count, false);
			BitArray localAssigned = new BitArray(ControlFlowGraph.Nodes.Count, false);
			HashSet<IRControlFlowGraphNode> unprocessedNodes = new HashSet<IRControlFlowGraphNode>();
			IRControlFlowGraphNode processingNode = null;
			for (int originalIndex = 0; originalIndex < originalCount; ++originalIndex)
			{
				phiInserted.SetAll(false);
				localAssigned.SetAll(false);
				foreach (IRControlFlowGraphNode node in ControlFlowGraph.Nodes)
				{
					if (node.SSAFinalIterations[originalIndex].Item2)
					{
						localAssigned.Set(node.Index, true);
						unprocessedNodes.Add(node);
					}
				}
				while (unprocessedNodes.Count > 0)
				{
					processingNode = unprocessedNodes.ElementAt(0);
					unprocessedNodes.Remove(processingNode);
					foreach (IRControlFlowGraphNode frontierNode in processingNode.Frontiers)
					{
						if (!phiInserted[frontierNode.Index])
						{
							tempLocal = Locals[originalIndex].Clone(this);
							Locals.Add(tempLocal);
							tempLocal.SSAData.Iteration = ++originalIterations[originalIndex];
							frontierNode.SSAPhis[originalIndex] = tempLocal;
							if (!frontierNode.SSAFinalIterations[originalIndex].Item2) frontierNode.SSAFinalIterations[originalIndex] = new Tuple<IRLocal, bool>(tempLocal, true);
							phiInserted.Set(frontierNode.Index, true);
							if (!localAssigned[frontierNode.Index])
							{
								localAssigned.Set(frontierNode.Index, true);
								unprocessedNodes.Add(frontierNode);
							}
						}
					}
				}
			}

			// Now we have assignments expanded, phi's created, and we can
			// determine phi sources from a nodes parents final iterations,
			// which we will use for leaving SSA later
			// Initial iterations for each original local in a node can now
			// be found through using the SSAPhi's if available, otherwise
			// using immediate dominator final iterations, the entry node
			// cannot have phis, and has no immediate dominator, so we just
			// copy the original locals (iteration 0) as the currentIterations
			// So finally, now we can retarget uses of original locals by
			// keeping track of the current iterations, replacing source uses
			// with current iterations, and updating the current iterations
			// when there is local assignments
			IRLocal[] currentIterations = new IRLocal[originalCount];
			foreach (IRControlFlowGraphNode node in ControlFlowGraph.Nodes)
			{
				if (node.Index == 0) Locals.CopyTo(0, currentIterations, 0, originalCount);
				else
				{
					for (int index = 0; index < originalCount; ++index)
						currentIterations[index] = node.SSAPhis[index] ?? node.Dominator.SSAFinalIterations[index].Item1;
				}
				foreach (IRInstruction instruction in node.Instructions)
				{
					instruction.Sources.ForEach(l => l.RetargetLocals(currentIterations));
					if (instruction.Destination != null && instruction.Destination.Type == IRLinearizedLocationType.Local)
					{
						tempLocal = Locals[instruction.Destination.Local.LocalIndex];
						currentIterations[tempLocal.SSAData.Original.Index] = tempLocal;
					}
				}
			}

		}

		public void LeaveSSA()
		{
		}
    }
}
