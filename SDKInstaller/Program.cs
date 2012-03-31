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

        private sealed class Download
        {
            public string DownloadFile = "";
            public ManualResetEvent DownloadEvent = new ManualResetEvent(false);
            public object DownloadLock = new object();
            public bool DownloadResult = false;
            public bool DownloadFinished = false;

            public Download(string pDownloadFile) { DownloadFile = pDownloadFile; }
        }

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

            if (!DownloadFile(string.Format("7za-{0}.exe", sHost))) return;
            sExtractorFile = string.Format("7za-{0}.exe", sHost);

            if (!DownloadFile(string.Format("SDK.7z"))) return;
            Extract(string.Format("SDK.7z"));

            if (!DownloadFile(string.Format("SDK-{0}.7z", sHost))) return;
            Extract(string.Format("SDK-{0}.7z", sHost));

            if (!DownloadFile(string.Format("SDK-{0}-{1}.7z", sTarget, sHost))) return;
            Extract(string.Format("SDK-{0}-{1}.7z", sTarget, sHost));

            Console.WriteLine();
            Console.WriteLine("Done, press any key to exit.");
            Console.Title = string.Format("SDKInstaller: Done");
            Console.ReadKey(true);
        }

        private static void DownloadFileCompleted(object pSender, AsyncCompletedEventArgs pArgs)
        {
            Download download = pArgs.UserState as Download;
            lock (download.DownloadLock)
            {
                download.DownloadResult = !pArgs.Cancelled;
                download.DownloadFinished = true;
                Console.WriteLine();
                download.DownloadEvent.Set();
            }
        }

        private static void DownloadProgressChanged(object pSender, DownloadProgressChangedEventArgs pArgs)
        {
            Download download = pArgs.UserState as Download;
            lock (download.DownloadLock)
            {
                if (!download.DownloadFinished)
                {
                    Console.CursorLeft = 0;
                    Console.Write("Progress {0,3}% [{1,-50}]", pArgs.ProgressPercentage, new string('.', pArgs.ProgressPercentage / 2));
                    Console.Title = string.Format("SDKInstaller: Downloading {0}, {1}%", download.DownloadFile, pArgs.ProgressPercentage);
                }
            }
        }

        private static bool DownloadFile(string pFilename)
        {
            WebClient client = new WebClient();
            client.DownloadFileCompleted += DownloadFileCompleted;
            client.DownloadProgressChanged += DownloadProgressChanged;

            Download download = new Download(pFilename);
            Console.WriteLine("Downloading {0}...", download.DownloadFile);
            download.DownloadEvent.Reset();
            client.DownloadFileAsync(new Uri(sURL + download.DownloadFile), download.DownloadFile, download);
            download.DownloadEvent.WaitOne();

            return download.DownloadResult;
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

            File.Delete(pFilename);
        }
    }
}
