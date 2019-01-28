#include <sstream>
#include "AccessDeniedException.h"

using libreg::AccessDeniedException;
using libreg::MultiString;
using libreg::SyscallFailure;

AccessDeniedException::AccessDeniedException(libreg::Hive hive, const MultiString& path, const SyscallFailure& inner) 
  : _hive(hive), _path(path), _inner(inner), _message(BuildMessage(hive, path, inner))
{
}

const char* AccessDeniedException::what() const noexcept
{
  return _message.c_str();
}

const MultiString& AccessDeniedException::Path() const
{
  return _path;
}

libreg::Hive AccessDeniedException::Hive() const
{
  return _hive;
}

const SyscallFailure& AccessDeniedException::Inner() const
{
  return _inner;
}

std::string AccessDeniedException::BuildMessage(libreg::Hive hive, const MultiString& path, const SyscallFailure& inner)
{
  std::stringstream str;
  str << "Access denied when accessing key: \"" << hive << "\\" << path << " not found. " << inner.what();

  return str.str();
}