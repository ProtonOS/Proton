using Proton.Plugs;
using System;
using System.IO;
using System.Collections.Generic;

namespace Proton.Core
{
    [PlugType(typeof(MonoIO))]
    public static class VirtualFileSystem
    {
        private static FileSystem[] sFileSystems = null;
        private static string sCurrentDirectory = "/";

        public static void Startup()
        {
            // TODO: Mount initfs to /, the boot directory to /boot, and the gac directory to /gac
            // initfs will contain /automount file for additional mounts
        }

        public static FileSystem GetFileSystem(string pPath)
        {
            FileSystem fs = null;
            for (int index = 0; index < sFileSystems.Length; ++index)
            {
                if (pPath.StartsWith(sFileSystems[index].Root) && (fs == null || fs.Root.Length < sFileSystems[index].Root.Length)) fs = sFileSystems[index];
            }
            return fs;
        }

        [PlugMethod]
        internal static bool CreateDirectory(string pPath, out MonoIOError pError)
        {
            FileSystem fs = GetFileSystem(pPath);
            if (fs == null)
            {
                pError = MonoIOError.ERROR_INVALID_DRIVE;
                return false;
            }
            pError = MonoIOError.ERROR_SUCCESS;
            FileSystem.IOError error;
            if (!fs.CreateDirectory(pPath, out error))
            {
                pError = (MonoIOError)error;
                return false;
            }
            return true;
        }

        [PlugMethod]
        internal static bool RemoveDirectory(string pPath, out MonoIOError pError)
        {
            FileSystem fs = GetFileSystem(pPath);
            if (fs == null)
            {
                pError = MonoIOError.ERROR_INVALID_DRIVE;
                return false;
            }
            pError = MonoIOError.ERROR_SUCCESS;
            FileSystem.IOError error;
            if (!fs.RemoveDirectory(pPath, out error))
            {
                pError = (MonoIOError)error;
                return false;
            }
            return true;
        }

        [PlugMethod]
        internal static string[] GetFileSystemEntries(string pPath, string pPathWithPattern, int pAttributes, int pMask, out MonoIOError pError)
        {
            FileSystem fs = GetFileSystem(pPath);
            if (fs == null)
            {
                pError = MonoIOError.ERROR_INVALID_DRIVE;
                return null;
            }
            FileSystem.IOError error;
            string[] entries = fs.GetEntries(pPath, pPathWithPattern, (FileAttributes)pAttributes, (FileAttributes)pMask, out error);
            pError = (MonoIOError)error;
            return entries;
        }

        [PlugMethod]
        internal static string GetCurrentDirectory(out MonoIOError pError)
        {
            // TODO: Might need to tie this into per process stuff
            pError = MonoIOError.ERROR_SUCCESS;
            return sCurrentDirectory;
        }

        [PlugMethod]
        internal static bool SetCurrentDirectory(string pPath, out MonoIOError pError)
        {
            // TODO: Might need to tie this into per process stuff
            pError = MonoIOError.ERROR_SUCCESS;
            if (!Directory.Exists(pPath))
            {
                pError = MonoIOError.ERROR_PATH_NOT_FOUND;
                return false;
            }
            sCurrentDirectory = pPath;
            return true;
        }

        [PlugMethod]
        internal static bool MoveFile(string pSourcePath, string pDestinationPath, out MonoIOError pError)
        {
            FileSystem fsSource = GetFileSystem(pSourcePath);
            if (fsSource == null)
            {
                pError = MonoIOError.ERROR_INVALID_DRIVE;
                return false;
            }
            FileSystem fsDestination = GetFileSystem(pDestinationPath);
            if (fsDestination == null)
            {
                pError = MonoIOError.ERROR_INVALID_DRIVE;
                return false;
            }
            pError = MonoIOError.ERROR_SUCCESS;
            FileSystem.IOError error;
            if (!fsSource.MoveFile(pSourcePath, fsDestination, pDestinationPath, out error))
            {
                pError = (MonoIOError)error;
                return false;
            }
            return true;
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

        [PlugMethod]
        internal static bool GetDiskFreeSpaceInternal(string pRootPath, out ulong pQuota, out ulong pTotal, out ulong pFree, out MonoIOError pError)
        {
            FileSystem fs = VirtualFileSystem.GetFileSystem(pRootPath);
            pQuota = 0;
            pTotal = 0;
            pFree = 0;
            if (fs == null)
            {
                pError = MonoIOError.ERROR_INVALID_DRIVE;
                return false;
            }
            pError = MonoIOError.ERROR_SUCCESS;
            pTotal = fs.TotalSpace;
            pFree = fs.FreeSpace;
            return true;
        }
    }
}
