#pragma once

#include <stdexcept>
#include "SyscallFailure.h"
#include "Hive.h"
#include "MultiString.h"
#include "ValueType.h"

namespace libreg
{
  class IncompatibleValueTypeException : public std::exception
  {
  public:
    IncompatibleValueTypeException(libreg::Hive hive, 
                                   const MultiString& path,
                                   ValueType expected,
                                   ValueType actual,
                                   const SyscallFailure& inner);

    const char* what() const noexcept override;
    
    const MultiString& Path() const;
    Hive Hive() const;
    const SyscallFailure& Inner() const;
    ValueType Expected() const;
    ValueType Actual() const;

  private:
    std::string BuildMessage(libreg::Hive hive, 
      const MultiString& path,
      ValueType expected,
      ValueType actual,
      const SyscallFailure& inner);

    libreg::Hive _hive;
    MultiString  _path;
    ValueType _expected;
    ValueType _actual;
    SyscallFailure _inner;
    
    std::string _message;
  };
}