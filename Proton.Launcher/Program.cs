using System;
using System.Diagnostics;

namespace Proton.Launcher
{
	internal static class Program
	{
		private static void Main()
		{
			ProcessStartInfo psi = new ProcessStartInfo(@".\SDK\qemu\qemu", @"-L .\SDK\qemu -smp 3 -cdrom .\Proton.iso -serial file:.\Logs\SymbolLog.txt -serial file:.\Logs\ConsoleLog.txt");
			psi.WorkingDirectory = @"..\..\..\";
			Process.Start(psi);
		}
	}
}
