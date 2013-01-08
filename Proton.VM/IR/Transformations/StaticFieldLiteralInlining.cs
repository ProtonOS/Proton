using System;
using System.Collections.Generic;
using Proton.Metadata.Signatures;

namespace Proton.VM.IR.Transformations
{
	public sealed class IRStaticFieldLiteralInliningTransformationPass : IRTransformationPass
	{
		public override TransformType Type { get { return TransformType.Method; } }

		public override void Transform(IRMethod method)
		{
			foreach (IRInstruction instruction in method.Instructions)
			{
				foreach (IRLinearizedLocation source in instruction.Sources)
				{
					if (source.Type == IRLinearizedLocationType.StaticField && source.StaticField.Field.IsLiteral)
					{
						switch (source.StaticField.Field.LiteralType)
						{
							case SigElementType.Boolean:
							case SigElementType.I1:
							case SigElementType.U1:
								source.Type = IRLinearizedLocationType.ConstantI4;
								source.ConstantI4.Value = source.StaticField.Field.LiteralValue[0];
								break;
							case SigElementType.I2:
							case SigElementType.U2:
							case SigElementType.Char:
								source.Type = IRLinearizedLocationType.ConstantI4;
								source.ConstantI4.Value = source.StaticField.Field.LiteralValue[0];
								source.ConstantI4.Value |= source.StaticField.Field.LiteralValue[1] << 8;
								break;
							case SigElementType.I4:
							case SigElementType.U4:
								source.Type = IRLinearizedLocationType.ConstantI4;
								source.ConstantI4.Value = source.StaticField.Field.LiteralValue[0];
								source.ConstantI4.Value |= source.StaticField.Field.LiteralValue[1] << 8;
								source.ConstantI4.Value |= source.StaticField.Field.LiteralValue[2] << 16;
								source.ConstantI4.Value |= source.StaticField.Field.LiteralValue[3] << 24;
								break;

							case SigElementType.I8:
							case SigElementType.U8:
								source.Type = IRLinearizedLocationType.ConstantI8;
								source.ConstantI8.Value = source.StaticField.Field.LiteralValue[0];
								source.ConstantI8.Value |= (long)source.StaticField.Field.LiteralValue[1] << 8;
								source.ConstantI8.Value |= (long)source.StaticField.Field.LiteralValue[2] << 16;
								source.ConstantI8.Value |= (long)source.StaticField.Field.LiteralValue[3] << 24;
								source.ConstantI8.Value |= (long)source.StaticField.Field.LiteralValue[4] << 32;
								source.ConstantI8.Value |= (long)source.StaticField.Field.LiteralValue[5] << 40;
								source.ConstantI8.Value |= (long)source.StaticField.Field.LiteralValue[6] << 48;
								source.ConstantI8.Value |= (long)source.StaticField.Field.LiteralValue[7] << 56;
								break;
							case SigElementType.R4:
								source.Type = IRLinearizedLocationType.ConstantR4;
								source.ConstantR4.Value = BitConverter.ToSingle(source.StaticField.Field.LiteralValue, 0);
								break;
							case SigElementType.R8:
								source.Type = IRLinearizedLocationType.ConstantR8;
								source.ConstantR8.Value = BitConverter.ToDouble(source.StaticField.Field.LiteralValue, 0);
								break;
							case SigElementType.IPointer:
							case SigElementType.UPointer:
								if (VMConfig.PointerSizeForTarget == 4)
								{
									source.Type = IRLinearizedLocationType.ConstantI4;
									source.ConstantI4.Value = source.StaticField.Field.LiteralValue[0];
									source.ConstantI4.Value |= source.StaticField.Field.LiteralValue[1] << 8;
									source.ConstantI4.Value |= source.StaticField.Field.LiteralValue[2] << 16;
									source.ConstantI4.Value |= source.StaticField.Field.LiteralValue[3] << 24;
								}
								else if (VMConfig.PointerSizeForTarget == 8)
								{
									source.Type = IRLinearizedLocationType.ConstantI8;
									source.ConstantI8.Value = source.StaticField.Field.LiteralValue[0];
									source.ConstantI8.Value |= (long)source.StaticField.Field.LiteralValue[1] << 8;
									source.ConstantI8.Value |= (long)source.StaticField.Field.LiteralValue[2] << 16;
									source.ConstantI8.Value |= (long)source.StaticField.Field.LiteralValue[3] << 24;
									source.ConstantI8.Value |= (long)source.StaticField.Field.LiteralValue[4] << 32;
									source.ConstantI8.Value |= (long)source.StaticField.Field.LiteralValue[5] << 40;
									source.ConstantI8.Value |= (long)source.StaticField.Field.LiteralValue[6] << 48;
									source.ConstantI8.Value |= (long)source.StaticField.Field.LiteralValue[7] << 56;
								}
								else throw new NotSupportedException();
								break;
							default: throw new NotSupportedException();
						}
						source.StaticField.Field = null;
					}
				}
			}
		}
	}
}
