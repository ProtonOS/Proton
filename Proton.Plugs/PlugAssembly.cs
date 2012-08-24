using System;

namespace Proton.Plugs
{
    /// <summary>
    /// This attribute is placed on assemblies which contain managed plugs.
    /// </summary>
    [AttributeUsage(AttributeTargets.Assembly)]
    public sealed class PlugAssembly : Attribute
    {
    }
}
