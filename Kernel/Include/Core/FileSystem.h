#pragma once

#include <string>

#include <Core/FileDescriptor.h>

namespace Core
{
    class FileSystem
    {
    public:
        virtual void OnRegister() = 0;
        virtual void OnUnregister() = 0;
        virtual const std::string& GetRoot() = 0;

        virtual int32_t Open(Core::FileDescriptor* pDescriptor, const std::string& pPath, int32_t pFlags, mode_t pMode) = 0;
    };
}
