#pragma once

#include <string>

#include <Core/FileDescriptor.h>

using namespace std;

namespace Core
{
    class FileSystem
    {
    public:
        virtual void OnRegister() = 0;
        virtual void OnUnregister() = 0;
        virtual const string& GetRoot() = 0;

        virtual int32_t Open(FileDescriptor* pDescriptor, const string& pPath, int32_t pFlags, mode_t pMode) = 0;
    };
}
