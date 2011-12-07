using System;
using System.IO;

namespace Proton.FileSystems
{
    public abstract class FileSystem
    {
        private string mRoot;

        protected FileSystem(string pRoot)
        {
            mRoot = pRoot;
        }

        public string Root { get { return mRoot; } }

        public abstract bool ReadOnly { get; }
        protected internal abstract FileDescriptor Open(string pFullPath, FileAccess pAccess, FileMode pMode);
        protected internal abstract bool Close(FileDescriptor pFileDescriptor);
        protected internal abstract void Mounted();
        protected internal abstract void Unmounted();
    }
}
