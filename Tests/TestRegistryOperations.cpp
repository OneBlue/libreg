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

  bool isElevated = false;

  libreg::Handle<HANDLE> token{ nullptr };

  if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, token.Addr()))
  {
    TOKEN_ELEVATION elevation{};
    DWORD size = sizeof(elevation);

    if (GetTokenInformation(token.Get(), TokenElevation, &elevation, sizeof(elevation), &size))
    {
      isElevated = elevation.TokenIsElevated;
    }
  }

  auto dummy_key = "Software";

  auto key = Key::OpenOrCreate(Hive::CurrentUser, dummy_key, Access::AllAccess);
  try
  {
    key.DeleteSubKey("libreg");
  }
  catch (const KeyNotFoundException&)
  {
  }

  key = Key::Create(Hive::CurrentUser, "Software\\libreg", Access::AllAccess, true);
  Test(Hive::CurrentUser, key.Hive(), "Key hive is correct");
  Test<std::wstring>(L"Software\\libreg", key.Path().Value(), "Key path is correct");
  Test(static_cast<DWORD>(Access::AllAccess), static_cast<DWORD>(key.Access()), "Key access is correct");

  key.SetValue("foo", "bar", ValueType::Sz);
  Test(true, std::wstring(L"bar") ==  key.GetValue<MultiString>("foo").Value(), "Read dword key");
  Test<std::wstring>(L"libreg", key.Name().Value(), "Key name is valid");

  key.SetValue("dword", 12, ValueType::Dword);
  Test<DWORD>(12, key.GetValue<DWORD>("dword"), "Read dword value");

  key.SetValue("empty", std::vector<uint8_t>(), ValueType::Sz);
  Test<std::string>("", key.GetValue<MultiString>("empty", libreg::ValueType::Sz).AsMultiByte(), "Read empty string value");

  key.SetValue("qword", 12, ValueType::Qword);
  Test<DWORDLONG>(12, key.GetValue<DWORDLONG>("qword", libreg::ValueType::Qword), "Read QWORD value");

  std::vector<uint8_t> binary{ 1, 3, 3, 7, 4, 2 };

  key.SetValue("binary", binary, ValueType::Binary);
  Test(true, binary == key.GetValue<std::vector<uint8_t>>("binary"), "Read binary value");

  std::vector<std::pair<MultiString, ValueType>> values =
  {
    {"foo", ValueType::Sz},
    {"qword", ValueType::Qword},
    {"empty", ValueType::Sz},
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
  Test<std::wstring>(L"Software\\libreg\\foo", subkey.Path().Value(), "Subkey path is correct");
  std::vector<MultiString> expected = { "foo" };

  Test(Path::Concat(key.Path(), "foo"), key.SubKeys().front().Path(), "Sub key is created");
  Test<size_t>(1, key.SubKeys().size(), "Only one Sub key is created");

  subkey.SetValue("", "default-value", ValueType::Sz);

  Test(true, std::wstring(L"default-value") == subkey.GetValue<MultiString>("").Value(), "Subkey default value is set");

  subkey.Delete();
  Test<size_t>(0, key.SubKeys().size(), "Subkey was deleted");

  TestThrow<KeyNotFoundException>([&](){subkey.Delete();}, "Keys can't be deleted twice");

  auto read_only_key = Key::Open(Hive::LocalMachine, isElevated ? "BCD00000000" : "Software", Access::QueryValue);

  TestThrow<AccessDeniedException>([&]() {read_only_key.CreateSubKey("denied"); },
    "AccessDeniedException is thrown");
  TestThrow<KeyNotFoundException>([&]() {read_only_key.OpenSubKey("does-not-exist", Access::Read); },
    "KeyNotFoundException is thrown");

  TestThrow<AccessDeniedException>([&]() {Key::Open(Hive::LocalMachine, "SAM\\SAM", Access::AllAccess); },
    "AccessDeniedException is thrown if key access is refused");

  auto top_level_key = Key::Open(Hive::LocalMachine, "", Access::Read);
  Test<std::wstring>(L"", top_level_key.Name().Value(), "Top level key name is valid");

  auto result = top_level_key.TryOpenSubKey("DoesntExist", Access::Read);
  Test(false, result.has_value(), "TryOpenSubKey doesn't fail if the key is not found");

  TestThrow<AccessDeniedException>([&]() {top_level_key.OpenSubKey("SAM\\SAM", Access::AllAccess);}, "TryOpenSubkey throws on AccessDenied");
}
