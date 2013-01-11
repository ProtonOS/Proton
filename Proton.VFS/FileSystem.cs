using System;

namespace Proton.VFS
{
	public abstract class FileSystem
	{
		public string Root { get; internal set; }

		public abstract bool CreateDirectory(string pPath, out FileSystemError pError);
		public abstract bool DeleteDirectory(string pPath, out FileSystemError pError);
		public abstract bool DirectoryExists(string pPath, out FileSystemError pError);

		public abstract string[] GetEntries(string pPath, out FileSystemError pError);

		public abstract bool CopyFile(string pSourcePath, FileSystem pDestinationFileSystem, string pDestinationPath, out FileSystemError pError);
		public abstract bool MoveFile(string pSourcePath, FileSystem pDestinationFileSystem, string pDestinationPath, out FileSystemError pError);
		public abstract bool DeleteFile(string pPath, out FileSystemError pError);
	}
}
