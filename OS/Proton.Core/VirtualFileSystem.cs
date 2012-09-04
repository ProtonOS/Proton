using Proton.Plugs;
using System;
using System.IO;
using System.Collections.Generic;

namespace Proton.Core
{
    [PlugType(typeof(MonoIO))]
    public static class VirtualFileSystem
    {
        private static Dictionary<string, FileSystem> sFileSystems = new Dictionary<string, FileSystem>();

        public static void Startup()
        {
        }

        public static FileSystem GetFileSystem(string pPath)
        {
            return null;
        }

        [PlugMethod]
        internal static bool CreateDirectory(string pPath, out MonoIOError pError)
        {
            pError = MonoIOError.ERROR_SUCCESS;
            return false;
        }

        [PlugMethod]
        internal static bool RemoveDirectory(string pPath, out MonoIOError pError)
        {
            pError = MonoIOError.ERROR_SUCCESS;
            return false;
        }
    }

    [PlugType(typeof(DriveInfo))]
    internal static class DriveInfoPlugs
    {
        [PlugMethod]
        internal static uint GetDriveTypeInternal(string pRootPath)
        {
            FileSystem fs = VirtualFileSystem.GetFileSystem(pRootPath);
            if (fs == null) return (uint)DriveType.Unknown;
            return (uint)fs.DriveType;
        }

        [PlugMethod]
        internal static string GetDriveFormat(string pRootPath)
        {
            FileSystem fs = VirtualFileSystem.GetFileSystem(pRootPath);
            if (fs == null) return "Unknown";
            return fs.DriveFormat;
        }
    }
}
