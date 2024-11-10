#include <iostream>
#include "Tester.h"

using std::cout;
using std::endl;

using libreg::test::Tester;

Tester::Tester(const std::string& category) : _category(category)
{
}

bool Tester::Run()
{
  cout << Colors::Blue() << Colors::Bold() << "\n\t\t\t======= Tests for \""
        << _category << "\" =======" << endl
        << Colors::Reset();

  try

  {
    RunImpl();
  }
  catch(const std::exception & e)
  {
    std::cout << Colors::Red() << "Tests caught unexpected exception: " << e.what() << std::endl;
    return false;
  }

  return _failed == 0;
}

void Tester::ShowResult() const
{
  cout << Colors::Blue() << Colors::Bold()
        << "Test results for category: \"" << _category << "\""
        << Colors::Reset() << endl;

  if (_failed == 0)
  {
    cout << "\t " << Colors::Green() << "all passed (" << _passed
          << " tests)\n";
  }
  else
  {
    cout << "\t Passed: " << _passed << endl;
    cout << "\t " << Colors::Red() << "Failed: " << _failed << endl;
  }

  cout << Colors::Reset() << '\n';
}

size_t Tester::Passed() const
{
  return _passed;
}

size_t Tester::Failed() const
{
  return _failed;
}