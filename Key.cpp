#include <map>
#include <cassert>
#include "Key.h"
#include "SyscallWrapper.h"
#include "Path.h"
#include "KeyNotFoundException.h"

using libreg::Key;
using libreg::MultiString;
using libreg::Access;
using libreg::Handle;
using libreg::Hive;
using libreg::ValueType;


Key::Key(Handle<HKEY>&& handle, libreg::Hive hive, const MultiString& path) 
  : _handle(std::make_unique<Handle<HKEY>>(std::move(handle))), _hive(hive), _path(path)
{

}

void Key::QueryInfo(const Handle<HKEY>& key,
                    size_t* max_subkey_chars,
                    size_t* max_value_name_chars,
                    size_t* max_data_bytes)
{
  DWORD dwMaxSubkey = 0;
  DWORD dwMaxValueNameLen = 0;
  DWORD dwMaxValueLen = 0;

  Syscall(RegQueryInfoKeyW,
    key.Get(),
    nullptr, // Class
    nullptr, // Class size
    nullptr, // Reserved
    nullptr, // Subkeys
    &dwMaxSubkey, // Max subkey name size
    nullptr, // Max class size
    nullptr, // Values count
    &dwMaxValueNameLen, // Max value name size
    &dwMaxValueLen, // Max value data size
    nullptr, // Security descriptor
    nullptr); // Last write time)

  auto assign = [](size_t* ptr, DWORD value)
  {
    if (ptr != nullptr)
    {
      *ptr = static_cast<size_t>(value);
    }
  };

  assign(max_subkey_chars, dwMaxSubkey);
  assign(max_value_name_chars, dwMaxValueNameLen);
  assign(max_data_bytes, dwMaxValueLen);
}

Key Key::Open(libreg::Hive hive, const MultiString& path, Access access)
{
  HKEY key = nullptr;
  try
  {
    Syscall(RegOpenKeyExW,
      reinterpret_cast<HKEY>(hive),
      path.Raw(),
      0,
      static_cast<DWORD>(access),
      &key);
  }
  catch (const SyscallFailure& e)
  {
    if (e.ReturnValue() == ERROR_FILE_NOT_FOUND)
    {
      throw KeyNotFoundException(hive, path, e);
    }
  }

  return Key(key, hive, path);
}

Key Key::Create(libreg::Hive hive, const MultiString& path, Access access, bool volatile_key)
{
  HKEY key = nullptr;

  Syscall(RegCreateKeyExW,
    reinterpret_cast<HKEY>(hive),
    path.Raw(),
    0,
    nullptr, //lpClass
    volatile_key ? REG_OPTION_VOLATILE : 0,
    static_cast<DWORD>(access), // regSamDesired
    nullptr, // lpSecurityAttributes
    &key,
    nullptr);

  return Key(key, hive, path);
}

Key Key::OpenOrCreate(libreg::Hive hive, const MultiString& path, Access access, bool volatile_key)
{
  try
  {
    return Open(hive, path, access);
  }
  catch (const KeyNotFoundException&)
  {
    return Create(hive, path, access, volatile_key);
  }
}

const Handle<HKEY>& Key::NativeHandle() const
{
  return *_handle;
}

std::vector<std::pair<MultiString, ValueType>> Key::Values() const
{
  size_t max_value_name = 0;
  QueryInfo(*_handle, nullptr, &max_value_name, nullptr);

  bool more_values = true;
  auto check_return = [&more_values](LSTATUS syscal_return)
  {
    if (syscal_return == ERROR_NO_MORE_ITEMS)
    {
      more_values = false;
      return true;
    }
    return syscal_return == ERROR_SUCCESS;
  };

  std::vector<std::pair<MultiString, ValueType>> values;
  for (DWORD i = 0; more_values; i++)
  {
    std::wstring name(max_value_name, '\0');
    DWORD size = static_cast<DWORD>(sizeof(wchar_t) * (name.size() + 1));
    DWORD type;

    SyscallWithExpectedReturn(RegEnumValueW,
      check_return,
      _handle->Get(),
      i,
      const_cast<wchar_t*>(name.data()),
      &size,
      nullptr,
      &type,
      nullptr,
      nullptr
    );

    if (more_values)
    {
      name.resize(size);
      values.emplace_back(name, static_cast<ValueType>(type));
    }
  }

  return values;
}

std::vector<Key> Key::SubKeys(Access access) const
{
  size_t max_subkey_size = 0;
  QueryInfo(*_handle, &max_subkey_size, nullptr, nullptr);

  bool more_keys = true;
  auto check_return = [&more_keys](LSTATUS syscal_return)
  {
    if (syscal_return == ERROR_NO_MORE_ITEMS)
    {
      more_keys = false;
      return true;
    }
    return syscal_return == ERROR_SUCCESS;
  };

  std::vector<Key> keys;
  for(DWORD index = 0; more_keys; index++)
  {
    std::wstring name(max_subkey_size, '\0');
    DWORD chars = static_cast<DWORD>(name.size() + 1);
    
    SyscallWithExpectedReturn(RegEnumKeyExW,
      check_return,
      _handle->Get(), // Hkey
      index, // dwIndw
      const_cast<wchar_t*>(name.data()), // lpName
      &chars, // lpcchName
      nullptr, // lpReserved
      nullptr, // lpClass
      nullptr, // lpcchClass
      nullptr); // lpftLastWriteTime

    if (more_keys)
    {
      name.resize(chars);
      keys.emplace_back(OpenSubKey(name, access));
    }
  }

  return keys;
}

Key Key::OpenSubKey(const MultiString& name, Access access) const
{
  HKEY subkey = nullptr;

  Syscall(RegOpenKeyExW,
    _handle->Get(),
    name.Raw(),
    0,
    static_cast<DWORD>(access),
    &subkey
  );

  return Key(subkey, _hive, Path::Concat(_path, name));
}

const MultiString& Key::Path() const
{
  return _path;
}

Hive Key::Hive() const
{
  return _hive;
}

void Key::SetValueImpl(const MultiString& name, const void* data, size_t size, ValueType type)
{
  Syscall(RegSetValueExW,
    _handle->Get(), // hKey
    name.Raw(), // lpValueName
    0, // reserved
    static_cast<DWORD>(type), // dwType,
    reinterpret_cast<const BYTE*>(data), //lpData,
    static_cast<DWORD>(size)); //cbData
}

void Key::GetValueImpl(const MultiString& name, void* data, size_t& size, ValueType expected_type)
{
  DWORD dwSize = static_cast<DWORD>(size);

  static const std::map<ValueType, DWORD> masks
  {
    {ValueType::None, RRF_RT_ANY},
    {ValueType::Dword, RRF_RT_REG_DWORD},
    {ValueType::Qword, RRF_RT_REG_QWORD},
    {ValueType::Binary, RRF_RT_REG_BINARY},
    {ValueType::Expand_sz, RRF_RT_REG_EXPAND_SZ},
    {ValueType::Sz, RRF_RT_REG_SZ},
    {ValueType::Multi_sz, RRF_RT_REG_MULTI_SZ},
  };

  auto mask = masks.find(expected_type);
  assert(mask != masks.end());

  Syscall(RegGetValueW,
    _handle->Get(), // hKey
    nullptr, // lpSubkey
    name.Raw(), //lpValue
    mask->second, // dwFlags
    nullptr, // pdwType
    data, // pvData
    &dwSize); // pcdData

  size = static_cast<size_t>(dwSize);
}

void Key::SetValue(const MultiString& name, DWORD value, ValueType type)
{
  SetValueImpl(name, &value, sizeof(DWORD), type);
}

void Key::SetValue(const MultiString& name, const std::vector<std::uint8_t>& value, ValueType type)
{
  SetValueImpl(name, &value, sizeof(DWORD), type);
}

void Key::SetValue(const MultiString& name, const MultiString& value, ValueType type)
{
  SetValueImpl(name, value.Raw(), sizeof(wchar_t) * (value.Value().size() + 1), type);
}

void Key::Delete()
{
  Syscall(RegDeleteTreeW,
    _handle->Get(),
    nullptr);
}

void Key::DeleteValue(const MultiString& name)
{
  Syscall(RegDeleteValueW,
    _handle->Get(),
    name.Raw()
  );
}