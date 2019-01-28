#include <map>
#include <cassert>
#include "ValueType.h"

using libreg::ValueType;

static const std::map<ValueType, std::string> mappings
{
  {ValueType::None, "None"},
  {ValueType::Sz, "Sz"},
  {ValueType::Dword, "Dword"},
  {ValueType::Qword, "Dword"},
  {ValueType::Multi_sz, "Multi_sz"},
  {ValueType::Expand_sz, "Expand_sz"},
  {ValueType::Binary, "binary"}
};

std::ostream& libreg::operator<<(std::ostream& stream, ValueType type)
{
  auto it = mappings.find(type);
  assert(it != mappings.end());

  return stream << it->second.c_str();
}