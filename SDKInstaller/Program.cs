using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Threading;
using System.Xml;
using System.Xml.Serialization;

namespace SDKInstaller
{
    internal static class Program
    {
        private static string sURL = "https://github.com/downloads/ProtonOS/Proton/";
        private static string sPackageList = "SDKPackages.xml";
        private static string sTarget = "i686-elf";
        private static string sHost = "i386-mingw32";
        private static string[] sExperimentals = new string[0];

        private static string sExtractorFile = "";
        private static string sDownloadFile = "";
        private static ManualResetEvent sDownloadEvent = new ManualResetEvent(false);
        private static object sDownloadLock = new object();
        private static bool sDownloadResult = false;

        [STAThread]
        private static void Main(string[] pCommandLine)
        {
            Arguments args = new Arguments(pCommandLine);
            if (args.Contains("url")) sURL = args["url"];
            if (args.Contains("target")) sTarget = args["target"];
            if (args.Contains("host")) sHost = args["host"];
            if (args.Contains("experimental")) sExperimentals = args["experimental"].Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);

            Console.WriteLine("Downloading {0}...", sPackageList);
            Console.Title = string.Format("SDKInstaller: Downloading {0}", sPackageList);
            WebClient client = new WebClient();
            try { client.DownloadFile(sURL + sPackageList, sPackageList); }
            catch (Exception exc)
            {
                Console.WriteLine("Failed: {0}", exc.Message);
                return;
            }
            SDKPackages installer = null;
            using (XmlReader reader = XmlReader.Create(sPackageList)) installer = (SDKPackages)(new XmlSerializer(typeof(SDKPackages))).Deserialize(reader);
            File.Delete(sPackageList);

            SDKPackages.Host host = installer.Hosts.Find(p => p.Name == sHost);
            if (host == null)
            {
                Console.WriteLine("There is no SDK environment available for host {0}", sHost);
                return;
            }
            SDKPackages.Host.Target target = host.Targets.Find(t => t.Name == sTarget);
            if (target == null)
            {
                Console.WriteLine("There is no SDK packages available for target {0} on host {1}", sTarget, sHost);
                return;
            }

            if (!Download(string.Format("7za-{0}.exe", sHost))) return;
            sExtractorFile = sDownloadFile;
            sDownloadResult = false;

            if (!Download(string.Format("Common-{0}.7z", sHost))) return;
            sDownloadResult = false;
            Extract(sDownloadFile);

            if (!Download(string.Format("SDK-{0}-{1}.7z", sTarget, sHost))) return;
            sDownloadResult = false;
            Extract(sDownloadFile);

            Console.WriteLine();
            Console.WriteLine("Done, press any key to exit.");
            Console.Title = string.Format("SDKInstaller: Done");
            Console.ReadKey(true);
        }

        private static void DownloadFileCompleted(object pSender, AsyncCompletedEventArgs pArgs)
        {
            lock (sDownloadLock)
            {
                sDownloadResult = !pArgs.Cancelled;
                Console.WriteLine();
                sDownloadEvent.Set();
            }
        }

        private static void DownloadProgressChanged(object pSender, DownloadProgressChangedEventArgs pArgs)
        {
            lock (sDownloadLock)
            {
                Console.CursorLeft = 0;
                Console.Write("Progress {0,3}% [{1,-50}]", pArgs.ProgressPercentage, new string('.', pArgs.ProgressPercentage / 2));
                Console.Title = string.Format("SDKInstaller: Downloading {0}, {1}%", sDownloadFile, pArgs.ProgressPercentage);
            }
        }

        private static bool Download(string pFilename)
        {
            WebClient client = new WebClient();
            client.DownloadFileCompleted += DownloadFileCompleted;
            client.DownloadProgressChanged += DownloadProgressChanged;

            sDownloadFile = pFilename;
            Console.WriteLine("Downloading {0}...", sDownloadFile);
            sDownloadEvent.Reset();
            client.DownloadFileAsync(new Uri(sURL + sDownloadFile), sDownloadFile);
            sDownloadEvent.WaitOne();

            return sDownloadResult;
        }

        private static void Extract(string pFilename)
        {
            Console.WriteLine("Extracting {0}...", pFilename);
            Console.Title = string.Format("SDKInstaller: Extracting {0}", pFilename);

            Process extract = new Process();
            extract.StartInfo.FileName = Path.GetFullPath(sExtractorFile);
            extract.StartInfo.Arguments = "x -y " + pFilename;
            extract.StartInfo.UseShellExecute = true;
            extract.Start();
            extract.WaitForExit();
        }
    }
}
