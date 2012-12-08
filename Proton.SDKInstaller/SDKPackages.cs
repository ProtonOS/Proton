using System;
using System.Collections.Generic;

namespace Proton.SDKInstaller
{
    public sealed class SDKPackages
    {
        public sealed class Package
        {
            public string Name = "";
            public DateTime Updated = DateTime.UtcNow;
        }

		public List<Package> HostPackages = new List<Package>();
		public List<Package> TargetPackages = new List<Package>();
	}
}
