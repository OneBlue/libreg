#include <sstream>
#include "IncompatibleValueTypeException.h"

using libreg::IncompatibleValueTypeException;
using libreg::MultiString;
using libreg::SyscallFailure;
using libreg::ValueType;

IncompatibleValueTypeException::IncompatibleValueTypeException(libreg::Hive hive,
  const MultiString& path,
  ValueType expected,
  ValueType actual,
  const SyscallFailure& inner)
  : _hive(hive),
  _path(path),
  _inner(inner),
  _expected(expected),
  _actual(actual),
  _message(BuildMessage(hive, path, expected, actual, inner))
{
}

const char* IncompatibleValueTypeException::what() const noexcept
{
  return _message.c_str();
}

const MultiString& IncompatibleValueTypeException::Path() const
{
  return _path;
}

libreg::Hive IncompatibleValueTypeException::Hive() const
{
  return _hive;
}

const SyscallFailure& IncompatibleValueTypeException::Inner() const
{
  return _inner;
}

ValueType IncompatibleValueTypeException::Expected() const
{
  return _expected;
}

ValueType IncompatibleValueTypeException::Actual() const
{
  return _actual;
}

std::string IncompatibleValueTypeException::BuildMessage(libreg::Hive hive,
  const MultiString& path,
  ValueType expected,
  ValueType actual,
  const SyscallFailure& inner)
{
  std::stringstream str;
  str << "Incompatible value type when accessing value: \"" << hive << "\\" << path 
    << ". Expected: " << _expected << ", actual: " << _actual << ", details: " << inner.what();

  return str.str();
}