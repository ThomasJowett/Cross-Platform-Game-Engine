using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Editor.DLLWrapper
{
    static class EngineAPI
    {
        private const string _dllName = "EngineDLL.dll";

        [DllImport(_dllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern int fnEngineDLL();

        public static int GetValueFromDLL()
        {
            return fnEngineDLL();
        }
    }
}
