using System;
using System.Collections.Generic;

namespace SDKInstaller
{
    public sealed class SDKPackages
    {
        public sealed class Host
        {
            public sealed class Target
            {
                public string Name = "";
                public DateTime Updated = DateTime.UtcNow;
            }

            public string Name = "";
            public DateTime Updated = DateTime.UtcNow;
            public List<Target> Targets = new List<Target>();
        }

        public List<Host> Hosts = new List<Host>();
    }
}
