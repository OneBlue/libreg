#pragma once

#include <cassert>
#include "Key.h"
namespace libreg
{
  template <>
  inline DWORD Key::GetValue<DWORD>(const MultiString& name, ValueType expected_type)
  {
    DWORD value = 0;
    size_t size = 0;

    GetValueImpl(name, &value, size, expected_type);

    return value;
  }

  template <>
  inline std::vector<std::uint8_t> Key::GetValue<std::vector<std::uint8_t>>(const MultiString& name, ValueType expected_type)
  {
    size_t max_value_size = 0;
    QueryInfo(*_handle, nullptr, nullptr, &max_value_size);

    std::vector<std::uint8_t> value(max_value_size);
    GetValueImpl(name, value.data(), max_value_size, expected_type);
    value.resize(max_value_size);
    return value;
  }

  template <>
  inline MultiString Key::GetValue<MultiString>(const MultiString& name, ValueType expected_type)
  {
    size_t max_value_size = 0;
    QueryInfo(*_handle, nullptr, nullptr, &max_value_size);

    std::wstring value(max_value_size, L'\0');
    GetValueImpl(name, const_cast<wchar_t*>(value.data()), max_value_size, expected_type);
    assert(max_value_size % (sizeof(wchar_t)) == 0);

    // Resize the string to its proper size
    value.resize((max_value_size - 1)/ sizeof(wchar_t));

    return value;
  }
}