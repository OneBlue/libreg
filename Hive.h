#pragma once
#include <Windows.h>
#include <stddef.h>

namespace libreg
{
  enum class Hive : uint64_t
  {
    root = reinterpret_cast<uint64_t>(HKEY_CLASSES_ROOT),
    local_machine = reinterpret_cast<uint64_t>(HKEY_LOCAL_MACHINE),
    users = reinterpret_cast<uint64_t>(HKEY_USERS),
    current_user = reinterpret_cast<uint64_t>(HKEY_CURRENT_USER),
    current_config = reinterpret_cast<uint64_t>(HKEY_CURRENT_CONFIG)
  };
}