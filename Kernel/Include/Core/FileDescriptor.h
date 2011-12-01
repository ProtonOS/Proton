#pragma once

#include <string>

namespace Core
{
    struct FileDescriptor
    {
        typedef uint32_t (*CloseHandler)(FileDescriptor* pDescriptor);
        typedef off_t (*LSeekHandler)(FileDescriptor* pDescriptor, off_t pOffset, int32_t pWhence);
        typedef uint32_t (*WriteHandler)(FileDescriptor* pDescriptor, const void* pData, size_t pLength);
        typedef uint32_t (*ReadHandler)(FileDescriptor* pDescriptor, void* pData, size_t pLength);

        uint8_t Index;
        bool Active;
        uint16_t Device;
        uint16_t BlockStart;
        uint16_t Mode;
        uint32_t TotalSize;
        uint32_t BlockSize;
        uint32_t BlockCount;
        bool TerminalStream;
        uint32_t Offset;
        std::string Path;
        CloseHandler Close;
        LSeekHandler LSeek;
        WriteHandler Write;
        ReadHandler Read;
    };
}
