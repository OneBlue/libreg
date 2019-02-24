#pragma once
#include <Windows.h>
#include <stddef.h>
#include <ostream>

namespace libreg
{
  enum class Hive : uint64_t
  {
    Root = reinterpret_cast<uint64_t>(HKEY_CLASSES_ROOT),
    LocalMachine = reinterpret_cast<uint64_t>(HKEY_LOCAL_MACHINE),
    Users = reinterpret_cast<uint64_t>(HKEY_USERS),
    CurrentUser = reinterpret_cast<uint64_t>(HKEY_CURRENT_USER),
    CurrentConfig = reinterpret_cast<uint64_t>(HKEY_CURRENT_CONFIG),
    PerformanceData = reinterpret_cast<uint64_t>(HKEY_PERFORMANCE_DATA),
  };

  std::ostream& operator<<(std::ostream& stream, Hive hive);
  std::wostream& operator<<(std::wostream& stream, Hive hive);
}