#include "callstack.h"

#include <exception>
#include <iostream>

using namespace std;

struct MyException : public exception {
  const char *what() const throw() { return "MyExcetpion"; }
};

int main(int argc, char const *argv[]) {
  try {
    throw MyException();
  } catch (const MyException &e) {
    cout << e.what() << "\n";
    cout
        << base::util::CallStack::currentExceptionCallstack.get()->debugString()
        << "\n";
  }
  return 0;
}
