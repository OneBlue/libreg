#pragma once

#include <stdexcept>

namespace libreg
{
  class SyscallFailure : public std::exception
  {
  public:
    SyscallFailure(const std::string& syscall, const std::string& arguments, long return_value, long last_error);

    const char* what() const noexcept override;
    long ReturnValue() const;
    long LastError() const;
    const std::string& Syscall() const;

  private:

    std::string _syscall;
    std::string _arguments;
    long _return_value = 0;
    long _last_error = 0;

    std::string _message;
  };
}

