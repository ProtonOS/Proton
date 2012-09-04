using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace Proton.Core
{
	public static class Interop
	{
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern object AddressAsObject(IntPtr pAddress);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern IntPtr ObjectAsAddress(object pObject);
    }
}
