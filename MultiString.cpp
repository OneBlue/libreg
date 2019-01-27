#include <codecvt>
#include "MultiString.h"

using libreg::MultiString;

MultiString::MultiString(const char* input) : _value(Convert(input))
{
}

MultiString::MultiString(const std::string& input) : _value(Convert(input))
{
}

MultiString::MultiString(const wchar_t* input) : _value(input)
{
}

MultiString::MultiString(const std::wstring& input) : _value(input)
{
}

const std::wstring& MultiString:: Value() const
{
  return _value;
}

const wchar_t* MultiString::Raw() const
{
  return _value.c_str();
}

std::wstring MultiString::Convert(const std::string& input)
{
  typedef std::codecvt_utf8<wchar_t> convert_type;
  std::wstring_convert<convert_type, wchar_t> converter;

  return converter.from_bytes(input);
}