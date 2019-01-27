#pragma once

#include <windows.h>
#include <Dbghelp.h>
#include <iostream>
#include <sstream>
#include <functional>

/* 
  This file contains the implementations details of Syscall.
  The point of this wrapper is be able to throw an exception with 
  all syscalls arguments, return value and last error upon failure

  This pattern avoid the classical C-way:
  int result = Function();
  if (result != SUCCESS)
  {
    // handle error
  }
  
  With this implementation, it's just:
  int result = Sycall(Function); // Will throw on error
*/


namespace libreg
{
  namespace detail
  {
    template <typename T>
    inline bool CheckReturnValue(T value)
    {
      static_assert(sizeof(T) != sizeof(T), "Return value type not supported");
    }

    template <>
    inline bool CheckReturnValue(HANDLE handle)
    {
      return handle != nullptr && handle != INVALID_HANDLE_VALUE;
    }

    template <>
    inline bool CheckReturnValue(HMENU handle)
    {
      return handle != nullptr && handle != INVALID_HANDLE_VALUE;
    }

    template <>
    inline bool CheckReturnValue(HINSTANCE instance)
    {
      return instance != nullptr;
    }

    template <>
    inline bool CheckReturnValue(HWND hwnd)
    {
      return hwnd != nullptr;
    }

    template <>
    inline bool CheckReturnValue(HDC dc)
    {
      return dc != nullptr;
    }

    template <>
    inline bool CheckReturnValue(BOOL value)
    {
      return value;
    }

    template <>
    inline bool CheckReturnValue(LSTATUS result)
    {
      return result == ERROR_SUCCESS;
    }

    template <>
    inline bool CheckReturnValue(FILE* value)
    {
      return value != nullptr;
    }

    inline bool InitSymbols()
    {
      static bool init = false;
      if (init)
      {
        return true;
      }

      init = SymInitialize(GetCurrentProcess(), nullptr, true);
      return init;
    }

    inline std::string SyscallName(const void* address)
    {
      if (!InitSymbols())
      {
        return "";
      }

      unsigned char name_buffer[sizeof(SYMBOL_INFO) + sizeof(TCHAR) * MAX_SYM_NAME] = { 0 };
      SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(name_buffer);
      symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
      symbol->MaxNameLen = MAX_SYM_NAME;

      if (!SymFromAddr(GetCurrentProcess(),
        reinterpret_cast<DWORD64>(address),
        nullptr,
        symbol))
      {
        std::cerr << "Failed to recover symbol: " << address << ", " << GetLastError() << std::endl;
        return "";
      }

      return std::string(symbol->Name, symbol->NameLen);
    }


    template <typename T>
    inline std::enable_if_t<!std::is_pointer<T>::value && !std::is_arithmetic<T>::value, std::string>  ValueToString(T)
    {
      return "[NonPrintable]";
    }

    template <>
    inline std::enable_if_t<!std::is_pointer<std::nullptr_t>::value && !std::is_arithmetic<std::nullptr_t>::value, std::string>  ValueToString(std::nullptr_t)
    {
      return "NULL";
    }

    template <typename T>
    inline std::enable_if_t<!std::is_pointer<T>::value && std::is_arithmetic<T>::value, std::string> ValueToString(T value)
    {
      return std::to_string(value);
    }

    template <typename T>
    inline std::enable_if_t<std::is_pointer<T>::value, std::string> ValueToString(T value)
    {
      std::ostringstream str;
      str << reinterpret_cast<const void*>(value);

      return str.str();
    }

    template <>
    inline std::enable_if_t<std::is_pointer<const char*>::value, std::string> ValueToString<const char*>(const char* value)
    {
      return "\"" + std::string(value) + "\"";
    }

    template <>
    inline std::enable_if_t<std::is_pointer<const wchar_t*>::value, std::string> ValueToString<const wchar_t*>(const wchar_t* value)
    {
      return "L\"" + std::string{ value, value + wcslen(value) } +"\"";
    }

    template <typename T>
    inline void PrintArgumentsImpl(std::ostream& out, T first)
    {
      out << ", " << ValueToString(first) << ")";
    }

    template <typename T, typename ...Args>
    inline void PrintArgumentsImpl(std::ostream& out, T first, Args... args)
    {
      out << ", " << ValueToString(first);

      PrintArgumentsImpl(out, std::forward<Args>(args)...);
    }

    template <typename T, typename ...Args>
    inline void PrintArguments(std::ostream& out, T first, Args... args)
    {
      out << "(" << ValueToString(first);
      PrintArgumentsImpl(out, std::forward<Args>(args)...);
    }

    template <typename T, typename ...Args>
    inline void PrintArguments(std::ostream& out, T first)
    {
      out << "(" << ValueToString(first) << ")";
    }

    template <typename Return, typename ...Args>
    inline void ThrowException(const void* address, Return ret, DWORD error, Args... args)
    {
      std::stringstream arguments;
      PrintArguments(arguments, std::forward<Args>(args)...);

      throw SyscallFailure(SyscallName(address), arguments.str(), ret, error);
    }

    template <typename Routine, typename Result, typename ...Args>
    inline typename std::result_of<Routine(Args...)>::type SyscallImpl(Routine routine,
      const std::function<bool(typename std::result_of<Routine(Args...)>::type)>& check_result,
      Args... args)
    {
      auto result = routine(args...);
      auto error = GetLastError();  // Has to be set now because CheckReturnValue might overwrite it

      if (!check_result(result))
      {
        detail::ThrowException(routine, result, error, std::forward<Args>(args)...);
      }

      return result;
    }
  }

  template <typename Routine, typename ...Args>
  inline typename std::result_of<Routine(Args...)>::type Syscall(Routine routine, Args... args)
  {
    using ReturnValue = typename std::result_of<Routine(Args...)>::type;

    return detail::SyscallImpl<Routine, ReturnValue, Args...>(routine, detail::CheckReturnValue<ReturnValue>, std::forward<Args>(args)...);
  }

  template <typename Routine, typename ...Args>
  inline typename std::result_of<Routine(Args...)>::type SyscallWithError(Routine routine,
    typename std::result_of<Routine(Args...)>::type error_value,
    Args... args)
  {
    using ReturnValue = typename std::result_of<Routine(Args...)>::type;

    auto pred = [&error_value](decltype(error_value) result)
    {
      return result != error_value;
    };

    return detail::SyscallImpl<Routine, ReturnValue, Args...>(routine, pred, std::forward<Args>(args)...);
  }

  template <typename Routine, typename ...Args>
  inline typename std::result_of<Routine(Args...)>::type SyscallWithExpectedReturn(Routine routine,
    typename std::result_of<Routine(Args...)>::type expected,
    Args... args)
  {
    using ReturnValue = typename std::result_of<Routine(Args...)>::type;

    auto pred = [&expected](decltype(expected) result)
    {
      return result == expected;
    };

    return detail::SyscallImpl<Routine, ReturnValue, Args...>(routine, pred, std::forward<Args>(args)...);
  }


  template <typename Routine, typename ...Args>
  inline typename std::result_of<Routine(Args...)>::type SyscallWithExpectedReturn(Routine routine,
    std::function<bool(typename std::result_of<Routine(Args...)>::type)> pred,
    Args... args)
  {
    using ReturnValue = typename std::result_of<Routine(Args...)>::type;

    return detail::SyscallImpl<Routine, ReturnValue, Args...>(routine, pred, std::forward<Args>(args)...);
  }
}