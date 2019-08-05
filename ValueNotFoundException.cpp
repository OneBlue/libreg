#include <sstream>
#include "ValueNotFoundException.h"

using libreg::ValueNotFoundException;
using libreg::MultiString;
using libreg::SyscallFailure;

ValueNotFoundException::ValueNotFoundException(libreg::Hive hive, const MultiString& path, const SyscallFailure& inner) 
  : _hive(hive), _path(path), _inner(inner), _message(BuildMessage(hive, path, inner))
{
}

const char* ValueNotFoundException::what() const noexcept
{
  return _message.c_str();
}

const MultiString& ValueNotFoundException::Path() const
{
  return _path;
}

libreg::Hive ValueNotFoundException::Hive() const
{
  return _hive;
}

const SyscallFailure& ValueNotFoundException::Inner() const
{
  return _inner;
}

std::string ValueNotFoundException::BuildMessage(libreg::Hive hive,
                                               const MultiString& path,
                                               const SyscallFailure& inner)
{
  std::stringstream str;
  str << "Value: \"" << hive << "\\" << path << "\" not found. " << inner.what();

  return str.str();
}