using Proton.Metadata;
using Proton.VM.IR;
using System;
using System.IO;

namespace Proton.Compiler
{
	internal static class Program
	{
		private static void Main(string[] pArguments)
		{
			Console.SetWindowSize(128, 40);
			Console.SetBufferSize(128, 1000);

			IRAppDomain appDomain = new IRAppDomain();
			IRAssembly entryAssembly = appDomain.LoadEntryAssembly(new CLIFile(Path.GetFileNameWithoutExtension(pArguments[0]), File.ReadAllBytes(pArguments[0])));

			Console.Write("Press any key to exit...");
			Console.ReadKey(true);
		}
	}
}
