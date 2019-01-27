#include <sstream>
#include "Path.h"

using libreg::MultiString;
using libreg::Path;

MultiString Path::Concat(const MultiString &left, const MultiString& right)
{
  std::wostringstream str;
  str << left.Value() << L"/" << right.Value();

  return str.str();
}