#include <algorithm>
#include <Key.h>
#include <ValueNotFoundException.h>
#include <IncompatibleValueTypeException.h>
#include <AccessDeniedException.h>
#include <KeyNotFoundException.h>
#include <Path.h>
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

  std::vector<uint8_t> binary{ 1, 3, 3, 7, 4, 2 };

  key.SetValue("binary", binary, ValueType::Binary);
  Test(true, binary == key.GetValue<std::vector<uint8_t>>("binary"), "Read binary value");

  std::vector<std::pair<MultiString, ValueType>> values =
  {
    {"foo", ValueType::Sz},
    {"binary", ValueType::Binary},
    {"dword", ValueType::Dword},
  };

  auto actual_values = key.Values();

  auto pred = [](const auto& left, const auto& right)
  {
    return left.first.Value() < right.first.Value();
  };

  std::sort(actual_values.begin(), actual_values.end(), pred);
  std::sort(values.begin(), values.end(), pred);
  Test(true, actual_values == values, "List key values");
  
  TestThrow<IncompatibleValueTypeException>([&]() {key.GetValue<MultiString>("dword", ValueType::Sz);},
                                           "IncompatibleValueTypeException is thrown when value type is incompatible");

  TestThrow<ValueNotFoundException>([&]() {key.GetValue<MultiString>("does-not-exists"); },
                                   "ValueNotFoundException is thrown if the value does not exist");

  key.DeleteValue("dword");
  TestThrow<ValueNotFoundException>([&]() {key.GetValue<DWORD>("dword"); }, "Deleted value is deleted");

  TestThrow<ValueNotFoundException>([&]() {key.DeleteValue("dword"); }, "Deleted value cannot be deleted again");


  auto subkey = key.CreateSubKey("foo", true);
  std::vector<MultiString> expected = { "foo" };

  Test(Path::Concat(key.Path(), "foo"), key.SubKeys().front().Path(), "Sub key is created");
  Test<size_t>(1, key.SubKeys().size(), "Only one Sub key is created");

  subkey.SetValue("", "default-value", ValueType::Sz);

  Test(true, std::wstring(L"default-value") == subkey.GetValue<MultiString>("").Value(), "Subkey default value is set");

  auto read_only_key = Key::Open(Hive::Local_machine, "Software", Access::query_value);

  TestThrow<AccessDeniedException>([&]() {read_only_key.CreateSubKey("denied"); },
    "AccessDeniedException is thrown");
  TestThrow<KeyNotFoundException>([&]() {read_only_key.OpenSubKey("does-not-exist", Access::read); },
    "KeyNotFoundException is thrown");

  TestThrow<AccessDeniedException>([&]() {Key::Open(Hive::Local_machine, "Software", Access::all_access); },
    "AccessDeniedException is thrown if key access is refused");
}
