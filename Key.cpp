#include <map>
#include <cassert>
#include "Key.h"
#include "SyscallWrapper.h"
#include "Path.h"
#include "KeyNotFoundException.h"
#include "AccessDeniedException.h"
#include "ValueNotFoundException.h"
#include "IncompatibleValueTypeException.h"

using libreg::Key;
using libreg::MultiString;
using libreg::Access;
using libreg::Handle;
using libreg::Hive;
using libreg::ValueType;
using libreg::SyscallFailure;
using libreg::KeyNotFoundException;
using libreg::AccessDeniedException;


Key::Key(Handle<HKEY>&& handle, libreg::Hive hive, const MultiString& path, libreg::Access access) 
  : _handle(std::make_unique<Handle<HKEY>>(std::move(handle))), _hive(hive), _path(path), _access(access)
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

Key Key::Open(libreg::Hive hive, const MultiString& path, libreg::Access access)
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
    HandleException(e, hive, path, false);
  }

  return Key(key, hive, path, access);
}

Key Key::Create(libreg::Hive hive, const MultiString& path, libreg::Access access, bool volatile_key)
{
  HKEY key = nullptr;

  try
  {
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
  }
  catch (const SyscallFailure& e)
  {
    HandleException(e, hive, path, false);
  }

  return Key(key, hive, path, access);
}

Key Key::OpenOrCreate(libreg::Hive hive, const MultiString& path, libreg::Access access, bool volatile_key)
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

std::vector<Key> Key::SubKeys(libreg::Access access) const
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

Key Key::OpenSubKey(const MultiString& name, libreg::Access access) const
{
  HKEY subkey = nullptr;

  try
  {
    Syscall(RegOpenKeyExW,
      _handle->Get(),
      name.Raw(),
      0,
      static_cast<DWORD>(access),
      &subkey
    );
  }
  catch (const SyscallFailure& e)
  {
    HandleException(e, _hive, Path::Concat(_path, name), false);
  }

  return Key(subkey, _hive, Path::Concat(_path, name), access);
}

Key Key::CreateSubKey(const MultiString& name, bool volatile_key)
{
  HKEY subkey = nullptr;
  try
  {
    Syscall(RegCreateKeyExW,
      _handle->Get(), //hKey
      name.Raw(), // lpSubkey
      0, // dwReserved
      nullptr, //lpClass
      volatile_key ? REG_OPTION_VOLATILE : 0, //dwOptions
      KEY_ALL_ACCESS, //samDesired
      nullptr, //lpSecurityAttributes
      &subkey, //phKey
      nullptr); // lpCreationDisposition
  }
  catch (const SyscallFailure& e)
  {
    HandleException(e, _hive, _path, false);
  }

  return Key(Handle<HKEY>(subkey), _hive, Path::Concat(_path, name), Access::AllAccess);
}

Key Key::OpenOrCreateSubkey(const MultiString& name, libreg::Access access, bool volatile_key)
{
  return OpenOrCreate(_hive, Path::Concat(_path, name), access, volatile_key);
}

const MultiString& Key::Path() const
{
  return _path;
}

MultiString libreg::Key::Name() const
{
  // Search for last '\' in string
  auto end = _path.Value().find_last_of('\\');

  if (end == std::string::npos)
  {
    return Path(); // If not found, then the key is a root key, return full path
  }

  return _path.Value().substr(end + 1);
}

Hive Key::Hive() const
{
  return _hive;
}

void Key::SetValueImpl(const MultiString& name, const void* data, size_t size, ValueType type)
{
  try
  {
    Syscall(RegSetValueExW,
      _handle->Get(), // hKey
      name.Raw(), // lpValueName
      0, // reserved
      static_cast<DWORD>(type), // dwType,
      reinterpret_cast<const BYTE*>(data), //lpData,
      static_cast<DWORD>(size)); //cbData
  }
  catch (const SyscallFailure& e)
  {
    HandleException(e, _hive, _path, true);
  }
}

void Key::GetValueImpl(const MultiString& name, void* data, size_t& size, ValueType expected_type) const
{
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
  
  DWORD dwSize = static_cast<DWORD>(size);
  DWORD dwType = 0;

  try
  {
    Syscall(RegGetValueW,
      _handle->Get(), // hKey
      nullptr, // lpSubkey
      name.Raw(), //lpValue
      mask->second | RRF_NOEXPAND, // dwFlags
      &dwType, // pdwType
      data, // pvData
      &dwSize); // pcdData
  }
  catch (const SyscallFailure& e)
  {
    if (e.ReturnValue() == ERROR_UNSUPPORTED_TYPE)
    {
      ValueType actualType = static_cast<ValueType>(dwType);
      throw IncompatibleValueTypeException(_hive, Path::Concat(_path, name), expected_type, actualType, e);
    }
    HandleException(e, _hive, Path::Concat(_path, name), true);
  }

  size = static_cast<size_t>(dwSize);
}

void Key::SetValue(const MultiString& name, DWORD value, ValueType type)
{
  SetValueImpl(name, &value, sizeof(DWORD), type);
}

void Key::SetValue(const MultiString& name, const std::vector<std::uint8_t>& value, ValueType type)
{
  SetValueImpl(name, value.data(), value.size(), type);
}

void Key::SetValue(const MultiString& name, const MultiString& value, ValueType type)
{
  SetValueImpl(name, value.Raw(), sizeof(wchar_t) * (value.Value().size() + 1), type);
}

void Key::DeleteSubKey(const MultiString& name)
{
  try
  {
    Syscall(RegDeleteTreeW,
      _handle->Get(),
      name.Raw());
  }
  catch (const SyscallFailure& e)
  {
    HandleException(e, _hive, _path, false);
  }
}

void Key::DeleteValue(const MultiString& name)
{
  try
  {
    Syscall(RegDeleteValueW,
      _handle->Get(), // hKey
      name.Raw() // lpSubKey
    );
  }
  catch (const SyscallFailure& e)
  {
    HandleException(e, _hive, _path, true);
  }
}

void Key::Delete()
{
  try
  {
    Syscall(RegDeleteTreeW,
      reinterpret_cast<HKEY>(_hive), // hKey
      _path.Raw() // lpSubKey
    );
  }
  catch (const SyscallFailure& e)
  {
    HandleException(e, _hive, _path, false);
  }
}

Access Key::Access() const
{
  return _access;
}

void Key::HandleException(const SyscallFailure& ex, libreg::Hive hive, const MultiString& path, bool value)
{
  if (ex.ReturnValue() == ERROR_FILE_NOT_FOUND)
  {
    if (value)
    {
      throw ValueNotFoundException(hive, path, ex);
    }
    else
    {
      throw KeyNotFoundException(hive, path, ex);
    }
  }
  else if (ex.ReturnValue() == ERROR_ACCESS_DENIED)
  {
    throw AccessDeniedException(hive, path, ex);
  }
  else
  {
    throw ex;
  }
}