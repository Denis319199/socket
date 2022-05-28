// No copyright

#include <iostream>

#include <fiber/Scheduler.hpp>

struct A {
  explicit A(int a) {
    std::cout << "A()\n";
    this->a = a;
  }

  ~A() { std::cout << "~A()\n"; }

  A(const A &other) {
    std::cout << "A(const A&)\n";
    a = other.a;
  }

  A(A &&other) {
    std::cout << "A(A&&)\n";
    a = other.a;
  }

  A &operator=(const A &other) {
    std::cout << "operator=(const A&)\n";
    a = other.a;
    return *this;
  }

  A &operator=(A &&other) {
    std::cout << "operator=(A&&)\n";
    a = other.a;
    return *this;
  }

  int a;
};

A Foo(A &&a) {
  a.a = 5;
  return static_cast<A &&>(a);
}

int main() {
  std::cout << '1' << std::endl;

  Scheduler::Schedule([]() {
    std::cout << '2' << std::endl;

    Scheduler::Yield();

    Scheduler::Schedule([]() { std::cout << '7' << std::endl; });

    std::cout << '4' << std::endl;
  });

  Scheduler::Yield();

  std::cout << '3' << std::endl;

  Scheduler::Schedule([]() { std::cout << '5' << std::endl; });

  Scheduler::Yield();

  std::cout << '6' << std::endl;

  Scheduler::Yield();

  std::cout << '8' << std::endl;

  Scheduler::Yield();

  std::cout << '9' << std::endl;

  return 0;
}
