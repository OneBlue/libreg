#include <sstream>
#include "SyscallFailure.h"

using libreg::SyscallFailure;

SyscallFailure::SyscallFailure(const std::string& syscall, const std::string& arguments, long return_value, long last_error): 
  _syscall(syscall), _arguments(arguments), _return_value(return_value), _last_error(last_error)
{
  std::stringstream str;

  str << "Syscall: \"" << _syscall << _arguments << "\" failed with error: " << _return_value;
  str << ", GetLastError() == " << _last_error;

  _message = str.str();
}

const char* SyscallFailure::what() const noexcept
{
  return _message.c_str();
}

long SyscallFailure::ReturnValue() const
{
  return _return_value;
}

long SyscallFailure::LastError() const
{
  return _last_error;
}

const std::string & libreg::SyscallFailure::Syscall() const
{
  return _syscall;
}
