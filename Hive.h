#pragma once
#include <Windows.h>
#include <stddef.h>
#include <ostream>

namespace libreg
{
  enum class Hive : uint64_t
  {
    Root = reinterpret_cast<uint64_t>(HKEY_CLASSES_ROOT),
    Local_machine = reinterpret_cast<uint64_t>(HKEY_LOCAL_MACHINE),
    Users = reinterpret_cast<uint64_t>(HKEY_USERS),
    Current_user = reinterpret_cast<uint64_t>(HKEY_CURRENT_USER),
    Current_config = reinterpret_cast<uint64_t>(HKEY_CURRENT_CONFIG)
  };

  std::ostream& operator<<(std::ostream& stream, Hive hive);
}