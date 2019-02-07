#include <map>
#include <cassert>
#include <stdio.h>
#include "Hive.h"

using libreg::Hive;

static const std::map<Hive, std::string> mappings
{
  {Hive::Root, "HKEY_CLASSES_ROOT"},
  {Hive::LocalMachine, "HKEY_LOCAL_MACHINE"},
  {Hive::Users, "HKEY_USERS"},
  {Hive::CurrentUser, "HKEY_CURRENT_USER"},
  {Hive::CurrentConfig, "HKEY_CURRENT_CONFIG"},
};

std::ostream& libreg::operator<<(std::ostream& stream, Hive hive)
{
  auto it = mappings.find(hive);
  assert(it != mappings.end());

  return (stream << it->second.c_str());
}