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

std::string MultiString::Convert(const std::wstring& input)
{
  typedef std::codecvt_utf8<wchar_t> convert_type;
  std::wstring_convert<convert_type, wchar_t> converter;

  return converter.to_bytes(input);
}

bool libreg::MultiString::operator==(const MultiString& other) const
{
  return Value() == other.Value();
}

bool libreg::MultiString::operator!=(const MultiString& other) const
{
  return !(*this == other);
}

std::ostream& libreg::operator<<(std::ostream& stream, const MultiString& string)
{
  return stream << MultiString::Convert(string.Value());
}