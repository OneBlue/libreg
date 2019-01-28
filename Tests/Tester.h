#pragma once

#include <string>
#include <functional>

namespace libreg
{
  namespace test
  {
    class Tester
    {
    public:
      Tester(const std::string& category);
      virtual ~Tester() = default;

      bool Run();
      virtual void RunImpl() = 0;
      void ShowResult() const;
      size_t Passed() const;
      size_t Failed() const;

    protected:
      template <typename T>
      bool Test(const T& result, const T& ref, const std::string& name);

      template <typename T>
      bool TestThrow(const std::function<void()>& routine, const std::string& name);

    private:
      size_t _passed = 0;
      size_t _failed = 0;

      std::string _category;
    };
  }
}

#include "Tester.hxx"
