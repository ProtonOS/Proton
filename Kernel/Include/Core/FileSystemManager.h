#pragma once

#include <string>
#include <map>
#include <array>

#include <Core/FileSystem.h>

namespace Core
{
    class FileSystemManager
    {
    public:
        static const uint16_t MaxDescriptors = 256;
        static const uint16_t ReservedDescriptors = 32;

        static bool Startup();
        static void Shutdown();

        static bool Register(Core::FileSystem* pFilesystem);
        static void Unregister(Core::FileSystem* pFilesystem);

        static Core::FileSystem* GetFileSystem(const std::string& pRoot);
        static Core::FileDescriptor* GetDescriptor(uint16_t pIndex);

    private:
        typedef std::map<std::string, Core::FileSystem*> FileSystemMap;
        typedef std::array<Core::FileDescriptor, MaxDescriptors> DescriptorArray;

        static FileSystemMap sFileSystems;
        static DescriptorArray sDescriptors;

        FileSystemManager();
        FileSystemManager(const FileSystemManager&);
    };
}
