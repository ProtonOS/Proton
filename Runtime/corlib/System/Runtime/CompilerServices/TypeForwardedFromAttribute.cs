
namespace System.Runtime.CompilerServices
{
    [AttributeUsage(AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Enum | AttributeTargets.Interface | AttributeTargets.Delegate)]
    public sealed class TypeForwardedFromAttribute : Attribute
    {
        private readonly string name;

        public TypeForwardedFromAttribute(string assemblyFullName)
        {
            name = assemblyFullName;
        }

        public string AssemblyFullName
        {
            get { return name; }
        }
    }
}