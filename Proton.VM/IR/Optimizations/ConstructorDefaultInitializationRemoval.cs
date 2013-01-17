using System;
using System.Collections.Generic;
using Proton.VM.IR.Instructions;

namespace Proton.VM.IR.Optimizations
{
	public sealed class IRConstructorDefaultInitializationRemovalOptimizationPass : IROptimizationPass
	{
		public override string Name { get { return "Constructor Default Initialization Removal"; } }
		public override string Description { get { return "Removes initialization of fields to their default value in constructors."; } }
		public override IROptimizationPass.RunLocation Location { get { return RunLocation.DuringSSA; } }

		private static bool IsDefaultValue(IRLinearizedLocation loc)
		{
			switch (loc.Type)
			{
				case IRLinearizedLocationType.ConstantI4:
					if (loc.ConstantI4.Value == 0)
						return true;
					return false;
				case IRLinearizedLocationType.ConstantI8:
					if (loc.ConstantI8.Value == 0L)
						return true;
					return false;
				case IRLinearizedLocationType.ConstantR4:
					if (loc.ConstantR4.Value == 0f)
						return true;
					return false;
				case IRLinearizedLocationType.ConstantR8:
					if (loc.ConstantR8.Value == 0d)
						return true;
					return false;
				case IRLinearizedLocationType.Null:
					return true;

				default:
					return false;
			}
		}

		public override void Run(IRMethod pMethod)
		{
			// instance or static constructors only.
			bool proc = false;
			bool st = false;
			if (pMethod.Name == ".ctor")
				proc = true;
			else if (pMethod.Name == ".cctor")
				proc = st = true;

			if (proc)
			{
				pMethod.Instructions.ImmediateRetargetModifiedInstructions = false;
				if (st)
				{
					for (int i = 0; i < pMethod.Instructions.Count - 1; i++)
					{
						var curInstr = pMethod.Instructions[i];
						if (
							curInstr.Opcode == IROpcode.Move &&
							IsDefaultValue(curInstr.Sources[0]) &&
							curInstr.Destination.Type == IRLinearizedLocationType.StaticField &&
							curInstr.Destination.StaticField.Field.ParentType == pMethod.ParentType
						)
						{
							pMethod.Instructions[i] = new IRNopInstruction();
						}
						else if (
							curInstr.Opcode == IROpcode.InitializeObject &&
							curInstr.Sources[0].Type == IRLinearizedLocationType.StaticFieldAddress &&
							curInstr.Sources[0].StaticFieldAddress.Field.ParentType == pMethod.ParentType
						)
						{
							pMethod.Instructions[i] = new IRNopInstruction();
						}
					}
				}
				else
				{
					for (int i = 0; i < pMethod.Instructions.Count - 1; i++)
					{
						var curInstr = pMethod.Instructions[i];
						if (
							curInstr.Opcode == IROpcode.Move &&
							IsDefaultValue(curInstr.Sources[0]) &&
							curInstr.Destination.Type == IRLinearizedLocationType.Field &&
							// This has to be a check for the param index because a constructor may use an
							// instance of itself. (yes, that is possible)
							curInstr.Destination.Field.FieldLocation.Type == IRLinearizedLocationType.Parameter &&
							curInstr.Destination.Field.FieldLocation.Parameter.ParameterIndex == 0
						)
						{
							pMethod.Instructions[i] = new IRNopInstruction();
						}
						else if (
							curInstr.Opcode == IROpcode.InitializeObject &&
							curInstr.Sources[0].Type == IRLinearizedLocationType.FieldAddress &&
							curInstr.Sources[0].FieldAddress.FieldLocation.Type == IRLinearizedLocationType.Parameter &&
							curInstr.Sources[0].FieldAddress.FieldLocation.Parameter.ParameterIndex == 0
						)
						{
							pMethod.Instructions[i] = new IRNopInstruction();
						}
					}
				}
				pMethod.Instructions.FixModifiedTargetInstructions();
				pMethod.Instructions.ImmediateRetargetModifiedInstructions = true;
			}
		}
	}
}
