extern "C" {
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
}

#include <Core/DeviceManager.h>
#include <Core/FileSystemManager.h>

using namespace Core;

FileSystemManager::FileSystemMap FileSystemManager::sFileSystems;
FileSystemManager::DescriptorArray FileSystemManager::sDescriptors;

uint32_t STDINRead(FileDescriptor* pDescriptor, void* pData, size_t pLength)
{
    // make a keyboard buffer, filled via interrupts, need keyboard maps from system files
    if (pDescriptor && pData && pLength) { }
    return 0;
}

uint32_t STDOUTWrite(FileDescriptor* pDescriptor, const void* pData, size_t pLength)
{
    if (pDescriptor) { }
    DeviceManager::GetConsole().WriteString(reinterpret_cast<const char*>(pData), pLength);
    return pLength;
}

uint32_t STDERRWrite(FileDescriptor* pDescriptor, const void* pData, size_t pLength)
{
    if (pDescriptor) { }
    DeviceManager::GetConsole().WriteString(reinterpret_cast<const char*>(pData), pLength);
    return pLength;
}

bool FileSystemManager::Startup()
{
    new(&sFileSystems) FileSystemMap;
    sDescriptors.fill(FileDescriptor());

    setbuf(stdin, nullptr);
    sDescriptors[STDIN_FILENO].Active = true;
    sDescriptors[STDIN_FILENO].Device = STDIN_FILENO;
    sDescriptors[STDIN_FILENO].Mode = S_IFCHR | ((S_IREAD | S_IWRITE) >> 3) | ((S_IREAD | S_IWRITE) >> 6);
    sDescriptors[STDIN_FILENO].TerminalStream = true;
    sDescriptors[STDIN_FILENO].Read = &STDINRead;

    setbuf(stdout, nullptr);
    sDescriptors[STDOUT_FILENO].Active = true;
    sDescriptors[STDOUT_FILENO].Device = STDOUT_FILENO;
    sDescriptors[STDOUT_FILENO].Mode = S_IFCHR | ((S_IREAD | S_IWRITE) >> 3) | ((S_IREAD | S_IWRITE) >> 6);
    sDescriptors[STDOUT_FILENO].TerminalStream = true;
    sDescriptors[STDOUT_FILENO].Write = &STDOUTWrite;

    setbuf(stderr, nullptr);
    sDescriptors[STDERR_FILENO].Active = true;
    sDescriptors[STDERR_FILENO].Device = STDERR_FILENO;
    sDescriptors[STDERR_FILENO].Mode = S_IFCHR | ((S_IREAD | S_IWRITE) >> 3) | ((S_IREAD | S_IWRITE) >> 6);
    sDescriptors[STDERR_FILENO].TerminalStream = true;
    sDescriptors[STDERR_FILENO].Write = &STDERRWrite;

    return true;
}

void FileSystemManager::Shutdown()
{
}

FileSystem* FileSystemManager::GetFileSystem(const string& pRoot)
{
    FileSystemMap::iterator it = sFileSystems.find(pRoot);
    if (it == sFileSystems.end()) return nullptr;
    return (*it).second;
}

FileDescriptor* FileSystemManager::GetDescriptor(uint16_t pIndex) { return &sDescriptors[pIndex]; }

bool FileSystemManager::Register(FileSystem* pFileSystem)
{
    if (sFileSystems.find(pFileSystem->GetRoot()) != sFileSystems.end()) return false;
    sFileSystems[pFileSystem->GetRoot()] = pFileSystem;
    return true;
}
