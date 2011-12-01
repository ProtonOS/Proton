#pragma once

#include <string>
#include <map>
#include <array>

#include <Core/FileSystem.h>

using namespace std;

namespace Core
{
    class FileSystemManager
    {
    public:
        static const uint16_t MaxDescriptors = 256;
        static const uint16_t ReservedDescriptors = 32;

        static bool Startup();
        static void Shutdown();

        static bool Register(FileSystem* pFilesystem);
        static void Unregister(FileSystem* pFilesystem);

        static FileSystem* GetFileSystem(const string& pRoot);
        static FileDescriptor* GetDescriptor(uint16_t pIndex);

    private:
        typedef map<string, FileSystem*> FileSystemMap;
        typedef array<FileDescriptor, MaxDescriptors> DescriptorArray;

        static FileSystemMap sFileSystems;
        static DescriptorArray sDescriptors;

        FileSystemManager();
        FileSystemManager(const FileSystemManager&);
    };
}
