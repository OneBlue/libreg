#pragma once

#include "MultiString.h"
namespace libreg
{
  class Path
  {
  public:
    static MultiString Concat(const MultiString& left, const MultiString& right);
  };
}