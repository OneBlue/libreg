#pragma once

#include <type_traits>
#include <functional>
#include <Windows.h>
#include "SyscallFailure.h"
#include "Defs.h"

namespace libreg
{
  template <typename Routine, typename ...Args>
  typename std::invoke_result_t<Routine, Args...> Syscall(Routine routine, Args... args);

  template <typename Routine, typename ...Args>
  typename  std::invoke_result_t<Routine, Args...>
  SyscallWithError(Routine routine, typename  std::invoke_result_t<Routine, Args...> error_value, Args... args);

  template <typename Routine, typename ...Args>
  typename  std::invoke_result_t<Routine, Args...>
  SyscallWithExpectedReturn(Routine routine, typename  std::invoke_result_t<Routine, Args...> expected_value, Args... args);

  template <typename Routine, typename ...Args>
  typename  std::invoke_result_t<Routine, Args...>
  SyscallWithExpectedReturn(Routine routine,
      std::function<bool(typename  std::invoke_result_t<Routine, Args...>)> pred, Args... args);

  template <typename T, typename... Args>
  void ComCall(const char* exp, HRESULT(T::*routine)(Args...), T* self, Args... args);
}
#include "SyscallWrapper.hxx"
