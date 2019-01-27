#pragma once

#include <type_traits>
#include <Windows.h>

namespace libreg
{
    enum class Access : DWORD
    {
        all_access = KEY_ALL_ACCESS,
        create_sub_key = KEY_CREATE_SUB_KEY,
        enumerate_sub_key = KEY_ENUMERATE_SUB_KEYS,
        read = KEY_READ,
        query_value = KEY_QUERY_VALUE,
        set_value = KEY_SET_VALUE,
        write = KEY_WRITE,
    };

    enum class Wow64Access : DWORD
    {
        key_32_bits = KEY_WOW64_32KEY,
        key_64_bits = KEY_WOW64_64KEY
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
}
