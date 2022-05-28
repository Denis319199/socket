// No copyright

#include <fiber/Scheduler.hpp>

Fiber::Fiber() : routine_{}, rsp_{} {}

Fiber::~Fiber() {
  if (!routine_.IsEmpty()) {
    StackPool::GetStackPool().Deallocate(rsp_);
  }
}

Fiber::Fiber(Routine &&routine)
    : routine_{std::move(routine)}, rsp_(StackPool::GetStackPool().Allocate()) {
  details::SetTrampoline(&rsp_, details::SmallTrampoline, Trampoline, this);
}

void Fiber::Trampoline(Fiber *fiber) {
  fiber->routine_();
  Scheduler::Return(fiber);
}

ExecutionContext::ExecutionContext(Fiber *fiber) : executing_fiber_{fiber} {}

void ExecutionContext::SwitchTo(Fiber *fiber) {
  auto prev_fiber = executing_fiber_;
  executing_fiber_ = fiber;

  details::SwitchContext(fiber->GetStackPointer(),
                         prev_fiber->GetStackPointer(),
                         Scheduler::ScheduleFiber, prev_fiber);
}

void ExecutionContext::SwitchToFinally(Fiber *fiber) {
  details::SwitchContextFinally(fiber->GetStackPointer());
}

void Scheduler::Yield() {
  auto next_fiber{GetNextFiber()};
  if (next_fiber) {
    execution_context_.SwitchTo(next_fiber);
  }
}

void Scheduler::ScheduleFiber(Fiber *fiber) { queue_.push(fiber); }

void Scheduler::Return(Fiber *fiber) {
  if (fiber != &main_fiber_) {
    delete fiber;
  }

  execution_context_.SwitchToFinally(GetNextFiber());
}

Fiber *Scheduler::GetNextFiber() {
  if (!queue_.empty()) {
    auto next_fiber{queue_.front()};
    queue_.pop();
    return next_fiber;
  }

  return nullptr;
}
