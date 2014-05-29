using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace Test
{
    class Program
    {
        [DllImport("AutoUpdateCommon.dll", CharSet = CharSet.Auto)]
        static extern void CheckVersion(IntPtr finddVersion);
        static void Main(string[] args)
        {

            
        }
    }
}
