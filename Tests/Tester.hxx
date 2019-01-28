#pragma once

#include <iostream>
#include "Tester.h"
#include "Colors.h"

using libreg::test::Tester;

template <typename T>
bool Tester::Test(const T& expected, const T& value, const std::string& name)
{
  if (expected != value)
  {
    std::cout << Colors::Red() << "FAILED: " << name << "\n" << Colors::Reset();

    std::cout << "\tExpected: \"" << expected << "\", actual: \"" << value << "\""
              << std::endl;
    _failed++;
    return false;
  }
  else
  {
    std::cout << Colors::Green() << "PASSED: " << name << "\n"
              << Colors::Reset();
    _passed++;
    return true;
  }
}

template <typename T>
bool Tester::TestThrow(const std::function<void()>& routine, const std::string& name)
{
  size_t failed = _failed;
  try
  {
    routine();

    std::cout << Colors::Red() << "FAILED: " << name
      << " did not throw as expected" << std::endl;
    _failed++;
    return false;
  }
  catch (const T& ex)
  {
    _passed++;
    std::cout << Colors::Green() << "PASSED: " << name << std::endl;
    std::cout << Colors::Reset();

    return true;
  }
}