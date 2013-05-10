using System;

namespace Proton.Runtime
{
	public static class RuntimeHelpers
	{
		internal static unsafe bool CanCast(object pObject, Type pType)
		{
			return true;
		}

		internal static unsafe Type GetTypeByGlobalTypeID(int pGlobalTypeID)
		{
			return null;
		}
	}
}
