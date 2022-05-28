// No copyright

#include <fiber/Routine.hpp>

Routine::Routine() : routine_wrapper_{} {}

Routine::Routine(Routine &&other) : routine_wrapper_{other.routine_wrapper_} {
  other.routine_wrapper_ = nullptr;
}

Routine &Routine::operator=(Routine &&other) {
  delete routine_wrapper_;
  routine_wrapper_ = other.routine_wrapper_;
  other.routine_wrapper_ = nullptr;

  return *this;
}

Routine::~Routine() { delete routine_wrapper_; }

void Routine::operator()() { routine_wrapper_->Run(); }

bool Routine::IsEmpty() {
  return !routine_wrapper_;
}
