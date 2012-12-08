namespace Proton.Metadata
{
    public static class CallingConvention
    {
	    public const byte Default		= 0x00;
	    public const byte CCall			= 0x01;
	    public const byte STDCall		= 0x02;
	    public const byte ThisCall		= 0x03;
	    public const byte FastCall		= 0x04;
	    public const byte VarArgs		= 0x05;
	    public const byte Generic		= 0x10;
	    public const byte HasThis		= 0x20;
        public const byte ExplicitThis  = 0x40;
    }
}
