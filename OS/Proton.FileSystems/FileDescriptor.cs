using System;
using System.IO;

namespace Proton.FileSystems
{
    public abstract class FileDescriptor
    {
        private FileSystem mFileSystem;
        private string mFullPath;

        protected FileDescriptor(FileSystem pFileSystem, string pFullPath)
        {
            mFileSystem = pFileSystem;
            mFullPath = pFullPath;
        }

        public FileSystem FileSystem { get { return mFileSystem; } }
        public string FullPath { get { return mFullPath; } }

        /*
        private ushort mDevice = 0;
        private ushort mBlockStart = 0;
        private ushort mMode = 0;
        private uint mTotalSize = 0;
        private uint mBlockSize = 0;
        private uint mBlockCount = 0;
        private bool mTerminalStream = false;

        public ushort Device { get { return mDevice; } protected set { mDevice = value; } }
        public ushort BlockStart { get { return mBlockStart; } protected set { mBlockStart = value; } }
        public ushort Mode { get { return mMode; } protected set { mMode = value; } }
        public uint TotalSize { get { return mTotalSize; } protected set { mTotalSize = value; } }
        public uint BlockSize { get { return mBlockSize; } protected set { mBlockSize = value; } }
        public uint BlockCount { get { return mBlockCount; } protected set { mBlockCount = value; } }
        public bool TerminalStream { get { return mTerminalStream; } protected set { mTerminalStream = value; } }
        */

        public bool Close()
        {
            if (!FileSystem.Close(this)) return false;
            VirtualFileSystem.Close(this);
            return true;
        }
        
        protected internal abstract int Seek(SeekOrigin pOrigin, int pOffset);
        protected internal abstract int Read(byte[] pData, int pLength);
        protected internal abstract int Write(byte[] pData, int pLength);
    }
}
