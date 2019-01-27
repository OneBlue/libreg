#pragma once

#include <Windows.h>

namespace libreg
{
  enum class ValueType
  {
    None = 0,
    Sz = REG_SZ,
    Dword = REG_DWORD,
    Qword = REG_QWORD,
    Multi_sz = REG_MULTI_SZ,
    Expand_sz = REG_EXPAND_SZ,
    Binary = REG_BINARY,
  };
}