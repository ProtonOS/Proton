#if !LOCALTEST

namespace System.IO {
	public static class Path {

		/*		static Path() {
					InvalidPathChars = GetInvalidPathChars();
					if (Environment.Platform == PlatformID.Unix) {
						AltDirectorySeparatorChar = '/';
						DirectorySeparatorChar = '/';
						PathSeparator = ';';
						VolumeSeparatorChar = '/';
					} else {
						AltDirectorySeparatorChar = '/';
						DirectorySeparatorChar = '\\';
						PathSeparator = ';';
						VolumeSeparatorChar = ':';
					}
					DirectorySeparatorStr = DirectorySeparatorChar.ToString();
					dirEqualsVolume = (DirectorySeparatorChar == VolumeSeparatorChar);
				}

				public static readonly char AltDirectorySeparatorChar;
				public static readonly char DirectorySeparatorChar;
				internal static readonly string DirectorySeparatorStr;
				public static readonly char[] InvalidPathChars;
				public static readonly char PathSeparator;
				public static readonly char VolumeSeparatorChar;
				private static bool dirEqualsVolume;

				public static char[] GetInvalidPathChars() {
					// return a new array as we do not want anyone to be able to change the values
					if (Environment.Platform == PlatformID.Unix) {
						return new char[] { '\x00' };
					} else {
						return new char[] {
							'\x22', '\x3C', '\x3E', '\x7C', '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07',
							'\x08', '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F', '\x10', '\x11', '\x12', 
							'\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', 
							'\x1E', '\x1F' };
					}
				}

				private static bool IsDsc(char c) {
					return c == DirectorySeparatorChar || c == AltDirectorySeparatorChar;
				}

				private static string WindowsDriveAdjustment(string path) {
					// two special cases to consider when a drive is specified
					if (path.Length < 2) {
						return path;
					}
					if ((path[1] != ':') || !Char.IsLetter(path[0])) {
						return path;
					}
					string current = Directory.GetCurrentDirectory();
					// first, only the drive is specified
					if (path.Length == 2) {
						// then if the current directory is on the same drive
						if (current[0] == path[0]) {
							path = current; // we return it
						} else {
							path += '\\';
						}
					} else if ((path[2] != Path.DirectorySeparatorChar) && (path[2] != Path.AltDirectorySeparatorChar)) {
						// second, the drive + a directory is specified *without* a separator between them (e.g. C:dir).
						// If the current directory is on the specified drive...
						if (current[0] == path[0]) {
							// then specified directory is appended to the current drive directory
							path = Path.Combine(current, path.Substring(2, path.Length - 2));
						} else {
							// if not, then just pretend there was a separator (Path.Combine won't work in this case)
							path = String.Concat(path.Substring(0, 2), DirectorySeparatorStr, path.Substring(2, path.Length - 2));
						}
					}
					return path;
				}

				public static bool IsPathRooted(string path) {
					if (path == null || path.Length == 0) {
						return false;
					}
					if (path.IndexOfAny(InvalidPathChars) != -1) {
						throw new ArgumentException("Illegal characters in path", "path");
					}
					char c = path[0];
					return (c == DirectorySeparatorChar ||
						c == AltDirectorySeparatorChar ||
						(!dirEqualsVolume && path.Length > 1 && path[1] == VolumeSeparatorChar));
				}

				static string CanonicalizePath(string path) {
					// STEP 1: Check for empty string
					if (path == null) {
						return path;
					}
					if (Environment.IsRunningOnWindows) {
						path = path.Trim();
					}
					if (path.Length == 0) {
						return path;
					}
					// STEP 2: Check to see if this is only a root
					string root = Path.GetPathRoot(path);
					// it will return '\' for path '\', while it should return 'c:\' or so.
					// Note: commenting this out makes the ened for the (target == 1...) check in step 5
					//if (root == path) return path;

					// STEP 3: split the directories, this gets rid of consecutative "/"'s
					string[] dirs = path.Split(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar);
					// STEP 4: Get rid of directories containing . and ..
					int target = 0;

					for (int i = 0; i < dirs.Length; i++) {
						if (dirs[i] == "." || (i != 0 && dirs[i].Length == 0)) {
							continue;
						} else if (dirs[i] == "..") {
							if (target != 0) {
								target--;
							}
						} else {
							dirs[target++] = dirs[i];
						}
					}

					// STEP 5: Combine everything.
					if (target == 0 || (target == 1 && dirs[0] == "")) {
						return root;
					} else {
						string ret = String.Join(DirectorySeparatorStr, dirs, 0, target);
						if (Environment.IsRunningOnWindows) {
							if (!SameRoot(root, ret)) {
								ret = root + ret;
							}
							// In GetFullPath(), it is assured that here never comes UNC. So this must only applied to such path that starts with '\', without drive specification.
							if (!IsDsc(path[0]) && SameRoot(root, path)) {
								if (ret.Length <= 2 && !ret.EndsWith(DirectorySeparatorStr)) { // '\' after "c:"
									ret += Path.DirectorySeparatorChar;
								}
								return ret;
							} else {
								string current = Directory.GetCurrentDirectory();
								if (current.Length > 1 && current[1] == Path.VolumeSeparatorChar) {
									// DOS local file path
									if (ret.Length == 0 || IsDsc(ret[0])) {
										ret += '\\';
									}
									return current.Substring(0, 2) + ret;
								} else if (IsDsc(current[current.Length - 1]) && IsDsc(ret[0])) {
									return current + ret.Substring(1);
								} else {
									return current + ret;
								}
							}
						}
						return ret;
					}
				}

				public static string GetFullPath(string path) {
					if (path == null) {
						throw new ArgumentNullException("path");
					}
					if (path.Trim().Length == 0) {
						string msg = "The specified path is not of a legal form (empty).";
						throw new ArgumentException(msg, "path");
					}

					// adjust for drives, i.e. a special case for windows
					if (Environment.IsRunningOnWindows) {
						path = WindowsDriveAdjustment(path);
					}
					// if the supplied path ends with a separator...
					char end = path[path.Length - 1];

					if (path.Length >= 2 && IsDsc(path[0]) && IsDsc(path[1])) {
						if (path.Length == 2 || path.IndexOf(path[0], 2) < 0) {
							throw new ArgumentException("UNC pass should be of the form \\\\server\\share.");
						}
						if (path[0] != DirectorySeparatorChar) {
							path = path.Replace(AltDirectorySeparatorChar, DirectorySeparatorChar);
						}
					} else {
						if (!IsPathRooted(path)) {
							path = Directory.GetCurrentDirectory() + DirectorySeparatorStr + path;
						} else if (DirectorySeparatorChar == '\\' && path.Length >= 2 && IsDsc(path[0]) && !IsDsc(path[1])) { // like `\abc\def'
							string current = Directory.GetCurrentDirectory();
							if (current[1] == VolumeSeparatorChar) {
								path = current.Substring(0, 2) + path;
							} else {
								path = current.Substring(0, current.IndexOf('\\', current.IndexOf("\\\\") + 1));
							}
						}
						path = CanonicalizePath(path);
					}

					// if the original ended with a [Alt]DirectorySeparatorChar then ensure the full path also ends with one
					if (IsDsc(end) && (path[path.Length - 1] != DirectorySeparatorChar)) {
						path += DirectorySeparatorChar;
					}

					return path;
				}*/

		public static readonly char[] InvalidPathChars;
		public static readonly char AltDirectorySeparatorChar;
		public static readonly char DirectorySeparatorChar;
		public static readonly char PathSeparator;
		internal static readonly string DirectorySeparatorStr;
		public static readonly char VolumeSeparatorChar;

		private static readonly char[] PathSeparatorChars;
		private static readonly bool dirEqualsVolume;

		// class methods
		public static string ChangeExtension(string path, string extension) {
			if (path == null)
				return null;

			if (path.IndexOfAny(InvalidPathChars) != -1)
				throw new ArgumentException("Illegal characters in path", "path");

			int iExt = findExtension(path);

			if (extension == null)
				return iExt < 0 ? path : path.Substring(0, iExt);
			else if (extension == String.Empty)
				return iExt < 0 ? path + '.' : path.Substring(0, iExt + 1);

			else if (path.Length != 0) {
				if (extension.Length > 0 && extension[0] != '.')
					extension = "." + extension;
			} else
				extension = String.Empty;

			if (iExt < 0) {
				return path + extension;
			} else if (iExt > 0) {
				string temp = path.Substring(0, iExt);
				return temp + extension;
			}

			return extension;
		}

		public static string Combine(string path1, string path2) {
			if (path1 == null)
				throw new ArgumentNullException("path1");

			if (path2 == null)
				throw new ArgumentNullException("path2");

			if (path1 == String.Empty)
				return path2;

			if (path2 == String.Empty)
				return path1;

			if (path1.IndexOfAny(InvalidPathChars) != -1)
				throw new ArgumentException("Illegal characters in path", "path1");

			if (path2.IndexOfAny(InvalidPathChars) != -1)
				throw new ArgumentException("Illegal characters in path", "path2");

			//TODO???: UNC names
			// LAMESPEC: MS says that if path1 is not empty and path2 is a full path
			// it should throw ArgumentException
			if (IsPathRooted(path2))
				return path2;

			char p1end = path1[path1.Length - 1];
			if (p1end != DirectorySeparatorChar && p1end != AltDirectorySeparatorChar && p1end != VolumeSeparatorChar)
				return path1 + DirectorySeparatorChar + path2;

			return path1 + path2;
		}

		public static string GetDirectoryName(string path) {
			// LAMESPEC: For empty string MS docs say both
			// return null AND throw exception.  Seems .NET throws.
			if (path == String.Empty) {
				throw new ArgumentException();
			}
			if (path == null || GetPathRoot(path) == path) {
				return null;
			}
			CheckArgument.WhitespaceOnly(path);
			CheckArgument.PathChars(path);

			int nLast = path.LastIndexOfAny(PathSeparatorChars);
			if (nLast == 0) {
				nLast++;
			}
			if (nLast > 0) {
				string ret = path.Substring(0, nLast);
				int l = ret.Length;
				if (l >= 2 && ret[l - 1] == VolumeSeparatorChar) {
					return ret + DirectorySeparatorChar;
				} else {
					return ret;
				}
			}

			return String.Empty;
		}

		public static string GetExtension(string path) {
			if (path == null) {
				return null;
			}
			if (path.IndexOfAny(InvalidPathChars) != -1) {
				throw new ArgumentException("Illegal characters in path", "path");
			}
			int iExt = findExtension(path);

			if (iExt > -1) {
				if (iExt < path.Length - 1) {
					return path.Substring(iExt);
				}
			}
			return string.Empty;
		}

		public static string GetFileName(string path) {
			if (path == null || path == String.Empty) {
				return path;
			}
			if (path.IndexOfAny(InvalidPathChars) != -1) {
				throw new ArgumentException("Illegal characters in path", "path");
			}
			int nLast = path.LastIndexOfAny(PathSeparatorChars);
			if (nLast >= 0) {
				return path.Substring(nLast + 1);
			}
			return path;
		}

		public static string GetFileNameWithoutExtension(string path) {
			return ChangeExtension(GetFileName(path), null);
		}

		public static string GetFullPath(string path) {
			string fullpath = InsecureGetFullPath(path);
			return fullpath;
		}

		internal static string WindowsDriveAdjustment(string path) {
			// two special cases to consider when a drive is specified
			if (path.Length < 2) {
				return path;
			}
			if ((path[1] != ':') || !Char.IsLetter(path[0])) {
				return path;
			}
			string current = Directory.GetCurrentDirectory();
			// first, only the drive is specified
			if (path.Length == 2) {
				// then if the current directory is on the same drive
				if (current[0] == path[0]) {
					path = current; // we return it
				} else {
					path += '\\';
				}
			} else if ((path[2] != Path.DirectorySeparatorChar) && (path[2] != Path.AltDirectorySeparatorChar)) {
				// second, the drive + a directory is specified *without* a separator between them (e.g. C:dir).
				// If the current directory is on the specified drive...
				if (current[0] == path[0]) {
					// then specified directory is appended to the current drive directory
					path = Path.Combine(current, path.Substring(2, path.Length - 2));
				} else {
					// if not, then just pretend there was a separator (Path.Combine won't work in this case)
					path = String.Concat(path.Substring(0, 2), DirectorySeparatorStr, path.Substring(2, path.Length - 2));
				}
			}
			return path;
		}

		// insecure - do not call directly
		internal static string InsecureGetFullPath(string path) {
			if (path == null) {
				throw new ArgumentNullException("path");
			}
			if (path.Trim().Length == 0) {
				string msg = "The specified path is not of a legal form (empty).";
				throw new ArgumentException(msg, "path");
			}

			// adjust for drives, i.e. a special case for windows
			if (Environment.IsRunningOnWindows) {
				path = WindowsDriveAdjustment(path);
			}
			// if the supplied path ends with a separator...
			char end = path[path.Length - 1];

			if (path.Length >= 2 &&
				IsDsc(path[0]) &&
				IsDsc(path[1])) {
				if (path.Length == 2 || path.IndexOf(path[0], 2) < 0) {
					throw new ArgumentException("UNC pass should be of the form \\\\server\\share.");
				}
				if (path[0] != DirectorySeparatorChar) {
					path = path.Replace(AltDirectorySeparatorChar, DirectorySeparatorChar);
				}
			} else {
				if (!IsPathRooted(path)) {
					path = Directory.GetCurrentDirectory() + DirectorySeparatorStr + path;
				} else if (DirectorySeparatorChar == '\\' &&
					path.Length >= 2 &&
					IsDsc(path[0]) &&
					!IsDsc(path[1])) { // like `\abc\def'
					string current = Directory.GetCurrentDirectory();
					if (current[1] == VolumeSeparatorChar) {
						path = current.Substring(0, 2) + path;
					} else {
						path = current.Substring(0, current.IndexOf('\\', current.IndexOf("\\\\") + 1));
					}
				}
				path = CanonicalizePath(path);
			}

			// if the original ended with a [Alt]DirectorySeparatorChar then ensure the full path also ends with one
			if (IsDsc(end) && (path[path.Length - 1] != DirectorySeparatorChar)) {
				path += DirectorySeparatorChar;
			}

			return path;
		}

		static bool IsDsc(char c) {
			return c == DirectorySeparatorChar || c == AltDirectorySeparatorChar;
		}

		public static string GetPathRoot(string path) {
			if (path == null)
				return null;

			if (path == String.Empty)
				throw new ArgumentException("This specified path is invalid.");

			if (!IsPathRooted(path))
				return String.Empty;

			if (DirectorySeparatorChar == '/') {
				// UNIX
				return IsDsc(path[0]) ? DirectorySeparatorStr : String.Empty;
			} else {
				// Windows
				int len = 2;

				if (path.Length == 1 && IsDsc(path[0]))
					return DirectorySeparatorStr;
				else if (path.Length < 2)
					return String.Empty;

				if (IsDsc(path[0]) && IsDsc(path[1])) {
					// UNC: \\server or \\server\share
					// Get server
					while (len < path.Length && !IsDsc(path[len])) len++;

					// Get share
					if (len < path.Length) {
						len++;
						while (len < path.Length && !IsDsc(path[len])) len++;
					}

					return DirectorySeparatorStr +
						DirectorySeparatorStr +
						path.Substring(2, len - 2).Replace(AltDirectorySeparatorChar, DirectorySeparatorChar);
				} else if (IsDsc(path[0])) {
					// path starts with '\' or '/'
					return DirectorySeparatorStr;
				} else if (path[1] == VolumeSeparatorChar) {
					// C:\folder
					if (path.Length >= 3 && (IsDsc(path[2]))) len++;
				} else
					return Directory.GetCurrentDirectory().Substring(0, 2);// + path.Substring (0, len);
				return path.Substring(0, len);
			}
		}

		public static string GetTempFileName() {
			throw new NotImplementedException();
		}

		/*public static string GetTempPath() {
			string p = get_temp_path();
			if (p.Length > 0 && p[p.Length - 1] != DirectorySeparatorChar)
				return p + DirectorySeparatorChar;

			return p;
		}

		//private static extern string get_temp_path();
		*/

		public static bool HasExtension(string path) {
			if (path == null || path.Trim() == String.Empty)
				return false;

			int pos = findExtension(path);
			return 0 <= pos && pos < path.Length - 1;
		}

		public static bool IsPathRooted(string path) {
			if (path == null || path.Length == 0)
				return false;

			if (path.IndexOfAny(InvalidPathChars) != -1)
				throw new ArgumentException("Illegal characters in path", "path");

			char c = path[0];
			return (c == DirectorySeparatorChar ||
				c == AltDirectorySeparatorChar ||
				(!dirEqualsVolume && path.Length > 1 && path[1] == VolumeSeparatorChar));
		}

		public static char[] GetInvalidFileNameChars ()
		{
			// return a new array as we do not want anyone to be able to change the values
			if (Environment.IsRunningOnWindows) {
				return new char [41] { '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07',
					'\x08', '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F', '\x10', '\x11', '\x12', 
					'\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', 
					'\x1E', '\x1F', '\x22', '\x3C', '\x3E', '\x7C', ':', '*', '?', '\\', '/' };
			} else {
				return new char [2] { '\x00', '/' };
			}
		}

		public static char[] GetInvalidPathChars ()
		{
			// return a new array as we do not want anyone to be able to change the values
			if (Environment.IsRunningOnWindows) {
				return new char [36] { '\x22', '\x3C', '\x3E', '\x7C', '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07',
					'\x08', '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F', '\x10', '\x11', '\x12', 
					'\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', 
					'\x1E', '\x1F' };
			} else {
				return new char [1] { '\x00' };
			}
		}

		public static string GetRandomFileName ()
		{
			throw new NotImplementedException();
		}
		// private class methods

		private static int findExtension(string path) {
			// method should return the index of the path extension
			// start or -1 if no valid extension
			if (path != null) {
				int iLastDot = path.LastIndexOf('.');
				int iLastSep = path.LastIndexOfAny(PathSeparatorChars);

				if (iLastDot > iLastSep)
					return iLastDot;
			}
			return -1;
		}

		static Path() {
			if (Environment.Platform == PlatformID.Unix) {
				AltDirectorySeparatorChar = '/';
				DirectorySeparatorChar = '/';
				PathSeparator = ';';
				VolumeSeparatorChar = '/';
			} else {
				AltDirectorySeparatorChar = '/';
				DirectorySeparatorChar = '\\';
				PathSeparator = ';';
				VolumeSeparatorChar = ':';
			}

			// this copy will be modifiable ("by design")
			InvalidPathChars = GetInvalidPathChars ();

			// internal fields
			DirectorySeparatorStr = DirectorySeparatorChar.ToString();
			PathSeparatorChars = new char[] {
				DirectorySeparatorChar,
				AltDirectorySeparatorChar,
				VolumeSeparatorChar
			};

			dirEqualsVolume = (DirectorySeparatorChar == VolumeSeparatorChar);
		}

		static bool SameRoot(string root, string path) {
			// compare root - if enough details are available
			if ((root.Length < 2) || (path.Length < 2))
				return false;
			// same volume/drive
			if (!root[0].Equals(path[0]))
				return false;
			// presence if the separator
			if (path[1] != Path.VolumeSeparatorChar)
				return false;
			if ((root.Length > 2) && (path.Length > 2)) {
				// but don't directory compare the directory separator
				return (IsDsc(root[2]) && IsDsc(path[2]));
			}
			return true;
		}

		static string CanonicalizePath(string path) {
			// STEP 1: Check for empty string
			if (path == null)
				return path;
			if (Environment.IsRunningOnWindows)
				path = path.Trim();

			if (path.Length == 0)
				return path;

			// STEP 2: Check to see if this is only a root
			string root = Path.GetPathRoot(path);
			// it will return '\' for path '\', while it should return 'c:\' or so.
			// Note: commenting this out makes the ened for the (target == 1...) check in step 5
			//if (root == path) return path;

			// STEP 3: split the directories, this gets rid of consecutative "/"'s
			string[] dirs = path.Split(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar);
			// STEP 4: Get rid of directories containing . and ..
			int target = 0;

			for (int i = 0; i < dirs.Length; i++) {
				if (dirs[i] == "." || (i != 0 && dirs[i].Length == 0))
					continue;
				else if (dirs[i] == "..") {
					if (target != 0)
						target--;
				} else
					dirs[target++] = dirs[i];
			}

			// STEP 5: Combine everything.
			if (target == 0 || (target == 1 && dirs[0] == ""))
				return root;
			else {
				string ret = String.Join(DirectorySeparatorStr, dirs, 0, target);
				if (Environment.IsRunningOnWindows) {
					if (!SameRoot(root, ret))
						ret = root + ret;
					// In GetFullPath(), it is assured that here never comes UNC. So this must only applied to such path that starts with '\', without drive specification.
					if (!IsDsc(path[0]) && SameRoot(root, path)) {
						if (ret.Length <= 2 && !ret.EndsWith(DirectorySeparatorStr)) // '\' after "c:"
							ret += Path.DirectorySeparatorChar;
						return ret;
					} else {
						string current = Directory.GetCurrentDirectory();
						if (current.Length > 1 && current[1] == Path.VolumeSeparatorChar) {
							// DOS local file path
							if (ret.Length == 0 || IsDsc(ret[0]))
								ret += '\\';
							return current.Substring(0, 2) + ret;
						} else if (IsDsc(current[current.Length - 1]) && IsDsc(ret[0]))
							return current + ret.Substring(1);
						else
							return current + ret;
					}
				}
				return ret;
			}
		}

		// required for FileIOPermission (and most proibably reusable elsewhere too)
		// both path MUST be "full paths"
		static internal bool IsPathSubsetOf(string subset, string path) {
			if (subset.Length > path.Length)
				return false;

			// check that everything up to the last separator match
			int slast = subset.LastIndexOfAny(PathSeparatorChars);
			if (String.Compare(subset, 0, path, 0, slast) != 0)
				return false;

			slast++;
			// then check if the last segment is identical
			int plast = path.IndexOfAny(PathSeparatorChars, slast);
			if (plast >= slast) {
				return String.Compare(subset, slast, path, slast, path.Length - plast) == 0;
			}
			if (subset.Length != path.Length)
				return false;

			return String.Compare(subset, slast, path, slast, subset.Length - slast) == 0;
		}

	}
}

#endif
