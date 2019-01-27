#include <iostream>
#include <Key.h>

using namespace libreg;

int main()
{
  auto key = Key::OpenOrCreate(Hive::current_user, "Software\\libreg", Access::all_access, true);

  key.SetValue("foo", "bar", ValueType::Sz);
  key.SetValue("fo2", 1337, ValueType::Dword);
  key.SetValue("fo3", std::vector<std::uint8_t>{1, 3, 3, 7}, ValueType::Binary);

  auto v = key.GetValue<MultiString>("foo");


  for (const auto& e : Key::Open(Hive::root, "*", Access::read).SubKeys())
  {
    std::wcout << e.Path().Value() << std::endl;
  }

  auto k2 = Key::Open(Hive::root, "*", Access::read);

  for (const auto& e : k2.Values())
  {
    std::wcout << e.first.Value() << "(" << (int)e.second << ")";
    std::wcout << " = " << k2.GetValue<MultiString>(e.first).Value() << std::endl;
  }

  return 0;
}