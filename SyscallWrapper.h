#pragma once

#include <type_traits>
#include <functional>
#include "SyscallFailure.h"

namespace libreg
{
  template <typename Routine, typename ...Args>
  typename std::result_of<Routine(Args...)>::type Syscall(Routine routine, Args... args);

  template <typename Routine, typename ...Args>
  typename std::result_of<Routine(Args...)>::type
    SyscallWithError(Routine routine, typename std::result_of<Routine(Args...)>::type error_value, Args... args);

  template <typename Routine, typename ...Args>
  typename std::result_of<Routine(Args...)>::type
    SyscallWithExpectedReturn(Routine routine, typename std::result_of<Routine(Args...)>::type expected_value, Args... args);

  template <typename Routine, typename ...Args>
  typename std::result_of<Routine(Args...)>::type
    SyscallWithExpectedReturn(Routine routine,
      std::function<bool(typename std::result_of<Routine(Args...)>::type)> pred, Args... args);
}
#include "SyscallWrapper.hxx"
