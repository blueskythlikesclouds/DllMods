#include "ShaderTranslator.h"

using namespace System;
using namespace Reflection;
using namespace IO;
using namespace Runtime::InteropServices;
using namespace ShaderTranslator;

Assembly^ OnAssemblyResolve(Object^ sender, ResolveEventArgs^ args)
{
    auto directoryPath = Path::GetDirectoryName(args->RequestingAssembly->Location);
    return Assembly::LoadFile(Path::Combine(directoryPath, (gcnew AssemblyName(args->Name))->Name + ".dll"));
}

void ShaderTranslatorService::init()
{
    AppDomain::CurrentDomain->AssemblyResolve += gcnew ResolveEventHandler(&OnAssemblyResolve);
}

void* ShaderTranslatorService::translate(void* function, int functionSize, int& translatedSize)
{
#define CATCH_ANYWAY 0

#if !_DEBUG || CATCH_ANYWAY
    try
    {
#endif
        auto bytes = Translator::Translate(function, functionSize);
        translatedSize = bytes->Length;

        return GCHandle::ToIntPtr(GCHandle::Alloc(bytes, GCHandleType::Pinned)).ToPointer();
#if !_DEBUG || CATCH_ANYWAY
    }
    catch (...)
    {
        return nullptr;
    }
#endif
}

void* ShaderTranslatorService::getBytes(void* handle)
{
    return GCHandle::FromIntPtr(IntPtr(handle)).AddrOfPinnedObject().ToPointer();
}

void ShaderTranslatorService::free(void* handle)
{
    GCHandle::FromIntPtr(IntPtr(handle)).Free();
}

int ShaderTranslatorService::getVersion()
{
    return Translator::Version;
}
