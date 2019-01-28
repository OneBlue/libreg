#pragma once

#include <stdexcept>
#include "SyscallFailure.h"
#include "Hive.h"
#include "MultiString.h"

namespace libreg
{
  class ValueNotFoundException : public std::exception
  {
  public:
    ValueNotFoundException(libreg::Hive hive, const MultiString& path, const SyscallFailure& inner);

    const char* what() const noexcept override;
    
    const MultiString& Path() const;
    Hive Hive() const;
    const SyscallFailure& Inner() const;

  private:
    static std::string BuildMessage(libreg::Hive hive, const MultiString& path, const SyscallFailure& inner);

    libreg::Hive _hive;
    MultiString  _path;
    SyscallFailure _inner;
    
    std::string _message;
  };
}