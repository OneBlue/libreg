#pragma once
#include <string>

namespace libreg
{
  class MultiString
  {
  public:
    MultiString(const char* input);
    MultiString(const wchar_t* input);
    MultiString(const std::string& input);
    MultiString(const std::wstring& input);

    const std::wstring& Value() const;
    const wchar_t* Raw() const;

  private:
    static std::wstring Convert(const std::string& input);

    std::wstring _value;
  };
}