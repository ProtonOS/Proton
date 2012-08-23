using System;

namespace Proton.Plugs
{
    [AttributeUsage(AttributeTargets.Class | AttributeTargets.Struct)]
    public sealed class PlugType : Attribute
    {
        public Type PlugTargetType;

        public PlugType(Type targetType)
        {
            this.PlugTargetType = targetType;
        }
    }
}
