#pragma once

#include <stdexcept>
#include "SyscallFailure.h"
#include "Hive.h"
#include "MultiString.h"

namespace libreg
{
  class AccessDeniedException : public std::exception
  {
  public:
    AccessDeniedException(libreg::Hive hive, const MultiString& path, const SyscallFailure& inner);

    const char* what() const noexcept override;
    
    const MultiString& Path() const;
    Hive Hive() const;
    const SyscallFailure& Inner() const;

  private:
    std::string BuildMessage(libreg::Hive hive, const MultiString& path, const SyscallFailure& inner);

    libreg::Hive _hive;
    MultiString  _path;
    SyscallFailure _inner;
    
    std::string _message;
  };
}