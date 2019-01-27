#pragma once

#include <stdexcept>
#include "SyscallFailure.h"
#include "Hive.h"
#include "MultiString.h"

namespace libreg
{
  class KeyNotFoundException : public std::exception
  {
  public:
    KeyNotFoundException(libreg::Hive hive, const MultiString& path, const SyscallFailure& inner);

    const char* what() const noexcept override;
    
    const MultiString& Path() const;
    Hive Hive() const;
    const SyscallFailure& Inner() const;

  private:
    libreg::Hive _hive;
    MultiString  _path;
    SyscallFailure _inner;
    
    std::string _message;
  };
}