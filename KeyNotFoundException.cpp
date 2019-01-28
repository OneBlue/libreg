#include <sstream>
#include "KeyNotFoundException.h"

using libreg::KeyNotFoundException;
using libreg::MultiString;
using libreg::SyscallFailure;

KeyNotFoundException::KeyNotFoundException(libreg::Hive hive, const MultiString& path, const SyscallFailure& inner) 
  : _hive(hive), _path(path), _inner(inner), _message(BuildMessage(hive, path, inner))
{
}

const char* KeyNotFoundException::what() const noexcept
{
  return _message.c_str();
}

const MultiString& KeyNotFoundException::Path() const
{
  return _path;
}

libreg::Hive KeyNotFoundException::Hive() const
{
  return _hive;
}

const SyscallFailure& KeyNotFoundException::Inner() const
{
  return _inner;
}

std::string KeyNotFoundException::BuildMessage(libreg::Hive hive,
                                               const MultiString& path,
                                               const SyscallFailure& inner)
{
  std::stringstream str;

  str << "Key: \"" << hive << "\\" << path << " not found. " << inner.what();

  return str.str();
}
