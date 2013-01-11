using System;
using System.Collections.Generic;

namespace Proton.VM.IR.Transformations
{
	public sealed class IRInstructionSpecializationTransformationPass : IRTransformationPass
	{
		public override IRTransformationPass.TransformType Type { get { return TransformType.Method; } }

		public override void Transform(IRMethod method)
		{
			method.Instructions.ImmediateRetargetModifiedInstructions = false;
			for (int i = 0; i < method.Instructions.Count; i++)
			{
				var curInstr = method.Instructions[i];
				switch (curInstr.Opcode)
				{
					case IROpcode.Unbox:
					{
						var curUBx = (Instructions.IRUnboxInstruction)curInstr;
						if (curUBx.GetValue && !curUBx.Type.IsValueType)
						{
							var c = new Instructions.IRCastInstruction(curUBx.Type, true);
							c.Sources.AddRange(curUBx.Sources);
							c.Sources.ForEach(s => s.SetParentInstruction(c));
							c.Destination = curUBx.Destination;
							c.Destination.SetParentInstruction(c);
							method.Instructions[i] = c;
						}
						break;
					}
					default:
						break;
				}
			}
			method.Instructions.FixModifiedTargetInstructions();
			method.Instructions.ImmediateRetargetModifiedInstructions = false;
		}
	}
}
