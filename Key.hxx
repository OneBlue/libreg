#pragma once

#include <cassert>
#include <algorithm>
#include "Key.h"
#include "SyscallWrapper.h"

namespace libreg
{
  template <>
  inline DWORD Key::GetValue<DWORD>(const MultiString& name, ValueType expected_type) const
  {
    DWORD value = 0;
    size_t size = sizeof(DWORD);

    GetValueImpl(name, &value, size, expected_type);
    assert(size == sizeof(DWORD));

    return value;
  }

  template <>
  inline DWORDLONG Key::GetValue<DWORDLONG>(const MultiString& name, ValueType expected_type) const
  {
    DWORDLONG value = 0;
    size_t size = sizeof(DWORDLONG);

    GetValueImpl(name, &value, size, expected_type);
    if (size == sizeof(DWORD))
    {
      return *reinterpret_cast<DWORD*>(&value);
    }

    assert(size == sizeof(DWORDLONG));

    return value;
  }

  template <>
  inline std::vector<std::uint8_t> Key::GetValue<std::vector<std::uint8_t>>(const MultiString& name, ValueType expected_type) const
  {
    size_t max_value_size = 0;
    QueryInfo(*_handle, nullptr, nullptr, &max_value_size);

    std::vector<std::uint8_t> value(max_value_size);
    GetValueImpl(name, value.data(), max_value_size, expected_type);
    value.resize(max_value_size);
    return value;
  }

  template <>
  inline MultiString Key::GetValue<MultiString>(const MultiString& name, ValueType expected_type) const
  {
    size_t max_value_size = GetValueSize(name);

    std::wstring value(max_value_size, L'\0');
    GetValueImpl(name, const_cast<wchar_t*>(value.data()), max_value_size, expected_type);
    assert(max_value_size % (sizeof(wchar_t)) == 0);

    // Resize the string to its proper size
    value.resize((max_value_size - 1)/ sizeof(wchar_t));

    return value;
  }
}