using System;
using System.Runtime.InteropServices;

namespace ShaderTranslator
{
    public static unsafe class Bridge
    {
        public delegate void OnProcessExitDelegate();

        [DllExport]
        public static void RegisterOnProcessExit(void* function)
        {
            AppDomain.CurrentDomain.ProcessExit += (sender, e) =>
            {
                Marshal.GetDelegateForFunctionPointer<OnProcessExitDelegate>((IntPtr)function)();
            };
        }

        [DllExport]
        public static void* Translate(void* function, int functionSize, out int translatedSize)
        {
            var bytes = Translator.Translate(function, functionSize);
            translatedSize = bytes.Length;

            return GCHandle.ToIntPtr(GCHandle.Alloc(bytes, GCHandleType.Pinned)).ToPointer();
        }

        [DllExport]
        public static void* Get(void* handle)
        {
            return GCHandle.FromIntPtr((IntPtr)handle).AddrOfPinnedObject().ToPointer();
        }

        [DllExport]
        public static void Free(void* handle)
        {
            GCHandle.FromIntPtr((IntPtr)handle).Free();
        }

        [DllExport]
        public static int Version()
        {
            return Translator.Version;
        }
    }
}
