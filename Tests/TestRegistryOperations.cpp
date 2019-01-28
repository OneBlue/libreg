#include <Key.h>
#include "TestRegistryOperations.h"

using namespace libreg;
using test::TestRegistryOperations;

TestRegistryOperations::TestRegistryOperations() : Tester("Registry operations")
{
}

void TestRegistryOperations::RunImpl()
{
  auto dummy_key = "Software";

  auto key = Key::OpenOrCreate(Hive::Current_user, dummy_key, Access::all_access);
  key.DeleteSubKey("libreg");

  key = Key::Create(Hive::Current_user, "Software\\libreg", Access::all_access, true);

  key.SetValue("foo", "bar", ValueType::Sz);
  Test(true, std::wstring(L"bar") ==  key.GetValue<MultiString>("foo").Value(), "Read dword key");

  key.SetValue("dword", 12, ValueType::Dword);

  Test<DWORD>(12, key.GetValue<DWORD>("dword"), "Read dword value");
}
