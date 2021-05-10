#include "callstack.h"

#include <exception>
#include <iostream>

#include "gtest/gtest.h"

using namespace std;

TEST(CallStack, get_normal_call_stack) {
  auto callstack = base::util::CallStack();
  cout << callstack.debugString() << "\n";
}

struct MyException : public exception {
  const char* what() const throw() { return "MyExcetpion"; }
};

// exception not working under gtest wrapping...
TEST(CallStack, get_exception_call_stack) {
  try {
    throw MyException();
  } catch (MyException& exception) {
    cout << exception.what() << "\n";
    cout
        << base::util::CallStack::currentExceptionCallstack.get()->debugString()
        << "\n";
  }
}
