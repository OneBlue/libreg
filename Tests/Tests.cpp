#include <iostream>
#include <Key.h>
#include "Colors.h"
#include "TestRegistryOperations.h"

using namespace libreg;
using namespace test;

static bool RunTestSuite(std::vector<std::unique_ptr<Tester>>& tests)
{
  size_t passed = 0;
  size_t failed = 0;

  bool success = true;
  for (auto& e : tests)
  {
    success &= e->Run();
  }

  std::cout << Colors::Blue() << Colors::Bold()
    << "\n\n\t\t\t======= Results: =======" << std::endl;

  for (auto& e : tests)
  {
    e->ShowResult();
    passed += e->Passed();
    failed += e->Failed();
  }

  if (failed == 0)
  {
    std::cout << Colors::Green();
  }
  else
  {
    std::cout << Colors::Red();
  }

  std::cout << Colors::Bold();

  std::cout << "Total: " << passed << " / " << failed + passed
    << Colors::Reset() << std::endl;

  return success;
}

int main()
{
  std::vector<std::unique_ptr<Tester>> tests;
  tests.emplace_back(std::make_unique<test::TestRegistryOperations>());

  return !RunTestSuite(tests);
}