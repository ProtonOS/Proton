using System;
using System.IO;
using System.Collections.Generic;

namespace Proton.FileSystems
{
    internal static class VirtualFileSystem
    {
        private const int MinimumFileDescriptors = 256;
        private const int MaximumFileDescriptors = 4096;
        private const int IncrementFileDescriptors = 32;
        private static List<FileSystem> sFileSystems = new List<FileSystem>();
        private static FileDescriptor[] sFileDescriptors = new FileDescriptor[MinimumFileDescriptors];

        internal static bool Mount(FileSystem pFileSystem)
        {
            if (sFileSystems.Exists(fs => fs.Root == pFileSystem.Root)) return false;

            sFileSystems.Insert(0, pFileSystem);
            pFileSystem.Mounted();
            return true;
        }

        internal static bool Unmount(FileSystem pFileSystem)
        {
            int fileSystemIndex = sFileSystems.IndexOf(pFileSystem);
            if (fileSystemIndex < 0) return false;

            for (int fileDescriptorIndex = 0; fileDescriptorIndex < sFileDescriptors.Length; ++fileDescriptorIndex)
            {
                if (sFileDescriptors[fileDescriptorIndex] != null &&
                    sFileDescriptors[fileDescriptorIndex].FileSystem == pFileSystem) return false;
            }
            sFileSystems.RemoveAt(fileSystemIndex);
            pFileSystem.Unmounted();
            return true;
        }

        public static FileDescriptor Open(string pFullPath, FileAccess pAccess, FileMode pMode)
        {
            FileSystem fileSystem = null;
            foreach (FileSystem fs in sFileSystems)
            {
                if (pFullPath.IndexOf(fs.Root) == 0)
                {
                    fileSystem = fs;
                    break;
                }
            }
            if (fileSystem == null) return null;
            if (fileSystem.ReadOnly && (pAccess != FileAccess.Read || pMode != FileMode.Open)) return null;
            int availableIndex = -1;
            for (int index = 0; index < sFileDescriptors.Length; ++index)
            {
                if (sFileDescriptors[index] == null)
                {
                    availableIndex = index;
                    break;
                }
            }
            if (availableIndex < 0 && sFileDescriptors.Length < MaximumFileDescriptors)
            {
                availableIndex = sFileDescriptors.Length;
                Array.Resize(ref sFileDescriptors, sFileDescriptors.Length + IncrementFileDescriptors);
            }
            if (availableIndex < 0) return null;

            FileDescriptor fileDescriptor = fileSystem.Open(pFullPath, pAccess, pMode);
            if (fileDescriptor == null) return null;

            sFileDescriptors[availableIndex] = fileDescriptor;
            return fileDescriptor;
        }

        internal static void Close(FileDescriptor pFileDescriptor)
        {
            int fileDescriptorIndex = Array.IndexOf(sFileDescriptors, pFileDescriptor);
            if (fileDescriptorIndex >= 0) sFileDescriptors[fileDescriptorIndex] = null;
        }
    }
}
