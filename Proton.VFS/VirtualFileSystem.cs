using System;
using System.Collections.Generic;
using System.IO;

namespace Proton.VFS
{
	public static class VirtualFileSystem
	{
		private static List<FileSystem> FileSystems = new List<FileSystem>();

		public static bool Mount(string pRoot, FileSystem pFileSystem)
		{
			FileSystemError fsError = FileSystemError.Success;
			if (pRoot != "/" && !DirectoryExists(pRoot, out fsError))
			{
				if (fsError != FileSystemError.Success) return false;
				if (!CreateDirectory(pRoot, out fsError)) return false;
			}
			if (FileSystems.Exists(fs => fs.Root == pRoot)) return false;
			pFileSystem.Root = pRoot;
			FileSystems.Add(pFileSystem);
			return true;
		}

		public static bool Unmount(string pRoot)
		{
			FileSystem fileSystem = FileSystems.Find(fs => fs.Root == pRoot);
			if (fileSystem == null) return false;
			FileSystems.Remove(fileSystem);
			return true;
		}

		public static FileSystem GetFileSystem(string pPath)
		{
			FileSystem fileSystem = null;
			foreach (FileSystem fs in FileSystems)
			{
				if (pPath.StartsWith(fs.Root) &&
					(fileSystem == null || fileSystem.Root.Length < fs.Root.Length))
				{
					fileSystem = fs;
				}
			}
			return fileSystem;
		}

		public static bool CreateDirectory(string pPath, out FileSystemError pError)
		{
			FileSystem fileSystem = GetFileSystem(pPath);
			if (fileSystem == null)
			{
				pError = FileSystemError.InvalidFileSystem;
				return false;
			}
			return fileSystem.CreateDirectory(pPath, out pError);
		}

		public static bool DeleteDirectory(string pPath, out FileSystemError pError)
		{
			FileSystem fileSystem = GetFileSystem(pPath);
			if (fileSystem == null)
			{
				pError = FileSystemError.InvalidFileSystem;
				return false;
			}
			return fileSystem.DeleteDirectory(pPath, out pError);
		}

		public static bool DirectoryExists(string pPath, out FileSystemError pError)
		{
			FileSystem fileSystem = GetFileSystem(pPath);
			if (fileSystem == null)
			{
				pError = FileSystemError.InvalidFileSystem;
				return false;
			}
			return fileSystem.DirectoryExists(pPath, out pError);
		}

		public static string[] GetEntries(string pPath, out FileSystemError pError)
		{
			FileSystem fileSystem = GetFileSystem(pPath);
			if (fileSystem == null)
			{
				pError = FileSystemError.InvalidFileSystem;
				return null;
			}
			return fileSystem.GetEntries(pPath, out pError);
		}

		public static bool CopyFile(string pSourcePath, string pDestinationPath, out FileSystemError pError)
		{
			FileSystem sourceFileSystem = GetFileSystem(pSourcePath);
			FileSystem destinationFileSystem = GetFileSystem(pDestinationPath);
			if (sourceFileSystem == null || destinationFileSystem == null)
			{
				pError = FileSystemError.InvalidFileSystem;
				return false;
			}
			return sourceFileSystem.CopyFile(pSourcePath, destinationFileSystem, pDestinationPath, out pError);
		}

		public static bool MoveFile(string pSourcePath, string pDestinationPath, out FileSystemError pError)
		{
			FileSystem sourceFileSystem = GetFileSystem(pSourcePath);
			FileSystem destinationFileSystem = GetFileSystem(pDestinationPath);
			if (sourceFileSystem == null || destinationFileSystem == null)
			{
				pError = FileSystemError.InvalidFileSystem;
				return false;
			}
			return sourceFileSystem.MoveFile(pSourcePath, destinationFileSystem, pDestinationPath, out pError);
		}

		public static bool DeleteFile(string pPath, out FileSystemError pError)
		{
			FileSystem fileSystem = GetFileSystem(pPath);
			if (fileSystem == null)
			{
				pError = FileSystemError.InvalidFileSystem;
				return false;
			}
			return fileSystem.DeleteFile(pPath, out pError);
		}
	}
}
