#pragma once
#include <string>
#include <ostream>

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
    std::string AsMultiByte() const;
    
    static std::string Convert(const std::wstring& input);
    bool operator==(const MultiString& other) const;
    bool operator!=(const MultiString& other) const;

  private:
    static std::wstring Convert(const std::string& input);

    std::wstring _value;
  };

  std::ostream& operator<<(std::ostream& stream, const MultiString& string);
}