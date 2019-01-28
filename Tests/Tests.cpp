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
  auto key = Key::OpenOrCreate(Hive::Current_user, "Software\\libreg", Access::all_access, true);

  key.SetValue("foo", "bar", ValueType::Sz);
  key.SetValue("fo2", 1337, ValueType::Dword);
  key.SetValue("fo3", std::vector<std::uint8_t>{1, 3, 3, 7}, ValueType::Binary);

  auto v = key.GetValue<MultiString>("foo");


  for (const auto& e : Key::Open(Hive::Root, "*", Access::read).SubKeys())
  {
    std::wcout << e.Path().Value() << std::endl;
  }

  auto k2 = Key::Open(Hive::Root, "*", Access::read);

  for (const auto& e : k2.Values())
  {
    std::wcout << e.first.Value() << "(" << (int)e.second << ")";
    std::wcout << " = " << k2.GetValue<MultiString>(e.first).Value() << std::endl;
  }

  std::vector<std::unique_ptr<Tester>> tests;
  tests.emplace_back(std::make_unique<test::TestRegistryOperations>());

  return !RunTestSuite(tests);
}