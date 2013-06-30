#pragma once

//Basic primitive types
typedef	signed char Int8;
typedef unsigned char UInt8;
typedef signed short Int16;
typedef unsigned short UInt16;
typedef signed int Int32;
typedef unsigned int UInt32;
typedef signed long long Int64;
typedef unsigned long long UInt64;
typedef bool Bool;
//Native sized int types equivalent to size of pointer
#ifdef IS64BIT
typedef Int64 Int;
typedef UInt64 UInt;
#else
typedef Int32 Int;
typedef UInt32 UInt;
#endif
//Some checks
static_assert(sizeof(Int8) == 1, "Incorrect Int8");
static_assert(sizeof(UInt8) == 1, "Incorrect UInt8");
static_assert(sizeof(Int16) == 2, "Incorrect Int16");
static_assert(sizeof(UInt16) == 2, "Incorrect UInt16");
static_assert(sizeof(Int32) == 4, "Incorrect Int32");
static_assert(sizeof(UInt32) == 4, "Incorrect UInt32");
static_assert(sizeof(Int64) == 8, "Incorrect Int64");
static_assert(sizeof(UInt64) == 8, "Incorrect UInt64");
static_assert(sizeof(Int) == sizeof(void *), "Incorrect Int");
static_assert(sizeof(UInt) == sizeof(void *), "Incorrect UInt");
static_assert(sizeof(Bool) == 1, "Incorrect Bool");

//Type information
template <typename T>
class Type
{
};
template <>
class Type<Int8>
{
public:
    constexpr static bool Signed = true;
    constexpr static UInt8 Bytes = 1;
    constexpr static UInt8 Bits = 8;
    constexpr static UInt8 Mask = 0;
    constexpr static UInt8 Shift = 0;
};
template <>
class Type<UInt8>
{
public:
    constexpr static bool Signed = false;
    constexpr static UInt8 Bytes = 1;
    constexpr static UInt8 Bits = 8;
    constexpr static UInt8 Mask = 0;
    constexpr static UInt8 Shift = 0;
};
template <>
class Type<Int16>
{
public:
    constexpr static bool Signed = true;
    constexpr static UInt8 Bytes = 2;
    constexpr static UInt8 Bits = 16;
    constexpr static UInt8 Mask = 1;
    constexpr static UInt8 Shift = 1;
};
template <>
class Type<UInt16>
{
public:
    constexpr static bool Signed = false;
    constexpr static UInt8 Bytes = 2;
    constexpr static UInt8 Bits = 16;
    constexpr static UInt8 Mask = 1;
    constexpr static UInt8 Shift = 1;
};
template <>
class Type<Int32>
{
public:
    constexpr static bool Signed = true;
    constexpr static UInt8 Bytes = 4;
    constexpr static UInt8 Bits = 32;
    constexpr static UInt8 Mask = 3;
    constexpr static UInt8 Shift = 2;
};
template <>
class Type<UInt32>
{
public:
    constexpr static bool Signed = false;
    constexpr static UInt8 Bytes = 4;
    constexpr static UInt8 Bits = 32;
    constexpr static UInt8 Mask = 3;
    constexpr static UInt8 Shift = 2;
};
template <>
class Type<Int64>
{
public:
    constexpr static bool Signed = true;
    constexpr static UInt8 Bytes = 8;
    constexpr static UInt8 Bits = 64;
    constexpr static UInt8 Mask = 7;
    constexpr static UInt8 Shift = 3;
};
template <>
class Type<UInt64>
{
public:
    constexpr static bool Signed = false;
    constexpr static UInt8 Bytes = 8;
    constexpr static UInt8 Bits = 64;
    constexpr static UInt8 Mask = 7;
    constexpr static UInt8 Shift = 3;
};
//Various utilities
namespace Core
{
    template <typename T>
    class RemoveConst
    {
    public:
        typedef T Type;
    };
    template <typename T>
    class RemoveConst<const T>
    {
    public:
        typedef T Type;
    };
}