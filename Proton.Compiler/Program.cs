using Proton.Metadata;
using Proton.VM.IR;
using System;
using System.IO;

namespace Proton.Compiler
{
	internal static class Program
	{
		public static string Target = "x86";
		public static string InputPath = "Proton.Nernel.exe";
		public static string OutputPath = "Proton.Nernel.bin";

		private static void Main(string[] pCommandLine)
		{
			bool pauseBeforeExit = false;

			Arguments args = new Arguments(pCommandLine);
			if (args.Contains("target")) Target = args["target"];
			if (args.Contains("input")) InputPath = args["input"];
			if (args.Contains("output")) OutputPath = args["output"];
			if (args.Contains("cwd")) Environment.CurrentDirectory = args["cwd"];
			if (args.Contains("pause")) pauseBeforeExit = true;

			string outputDirectory = Path.GetDirectoryName(OutputPath);
			if (!string.IsNullOrWhiteSpace(outputDirectory) && !Directory.Exists(outputDirectory)) Directory.CreateDirectory(outputDirectory);
			// TODO: Remove this when there is a binary file written out
			File.WriteAllBytes(OutputPath, new byte[0]);

			IRAppDomain appDomain = new IRAppDomain();
			IRAssembly entryAssembly = appDomain.LoadEntryAssembly(new CLIFile(Path.GetFileNameWithoutExtension(InputPath), File.ReadAllBytes(InputPath)));

			if (pauseBeforeExit)
			{
				Console.Write("Press any key to exit...");
				Console.ReadKey(true);
			}
		}
	}
}
