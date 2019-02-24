#pragma once

#include <type_traits>
#include <Windows.h>

namespace libreg
{
    enum class Access : DWORD
    {
        AllAccess = KEY_ALL_ACCESS,
        CreateSubKey = KEY_CREATE_SUB_KEY,
        EnumerateSubKey = KEY_ENUMERATE_SUB_KEYS,
        Read = KEY_READ,
        QueryValue = KEY_QUERY_VALUE,
        SetValue = KEY_SET_VALUE,
        Write = KEY_WRITE,
    };

    enum class Wow64Access : DWORD
    {
        Key32Bits = KEY_WOW64_32KEY,
        Key64Bits = KEY_WOW64_64KEY
    };

    template <typename TLeft, typename TRight>
    struct IsAccessFlag
    {
        static const bool value = false;
    };

    template <>
    struct IsAccessFlag<Access, Access>
    {
        static const bool value = true;
    };

    template <>
    struct IsAccessFlag<Wow64Access, Access>
    {
        static const bool value = true;
    };

    template <>
    struct IsAccessFlag<Access, Wow64Access>
    {
        static const bool value = true;
    };

    template <typename TLeft, typename TRight>
    std::enable_if_t<IsAccessFlag<TLeft, TRight>::value, Access> operator| (TLeft left, TRight right)
    {
        return static_cast<Access>(static_cast<DWORD>(left) | static_cast<DWORD>(right));
    }

    template <typename TLeft, typename TRight>
    std::enable_if_t<IsAccessFlag<TLeft, TRight>::value, bool> operator& (TLeft left, TRight right)
    {
      return static_cast<DWORD>(left) & static_cast<DWORD>(right);
    }
}
