using System;

namespace Proton.Plugs
{
    [AttributeUsage(AttributeTargets.Constructor | AttributeTargets.Method)]
    public sealed class PlugMethod : Attribute
    {
    }
}
