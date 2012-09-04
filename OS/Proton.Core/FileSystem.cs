using System;
using System.IO;
using System.Collections.Generic;

namespace Proton.Core
{
    public abstract class FileSystem
    {
        public enum IOError : uint
        {
            Success = 0x00,
            FileNotFound = 0x02,
            PathNotFound = 0x03,
            TooManyOpenFiles = 0x04,
            AccessDenied = 0x05,
            InvalidHandle = 0x06,
            InvalidDrive = 0x0F,
            NotSameDevice = 0x11,
            NoMoreFiles = 0x12,
            WriteFault = 0x1D,
            ReadFault = 0x1E,
            GeneralFailure = 0x1F,
            SharingViolation = 0x20,
            LockViolation = 0x21,
            DiskFull = 0x27,
            FileExists = 0x50,
            CannotMake = 0x52,
            InvalidParameter = 0x57,
            BrokenPipe = 0x6B,
            InvalidName = 0x7B,
            DirectoryNotEmpty = 0x91,
            AlreadyExists = 0xB7,
            FilenameExceedsRange = 0xCE,
            EncryptionFailed = 0x1770,
        }

        private string mRoot = null;

        public string Root { get { return mRoot; } internal set { mRoot = value; } }

        internal abstract bool CreateDirectory(string pPath, out IOError pError);
        internal abstract bool RemoveDirectory(string pPath, out IOError pError);
    }
}
