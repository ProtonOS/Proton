using Proton.Plugs;
using System;
using System.IO;
using System.Collections.Generic;

namespace Proton.Core
{
    [PlugType(typeof(MonoIO))]
    internal static class VirtualFileSystem
    {
        public static void Startup()
        {
        }


        public static bool CreateDirectory(string pPath, out MonoIOError pError)
        {
            pError = MonoIOError.ERROR_SUCCESS;
            return false;
        }
    }
}
