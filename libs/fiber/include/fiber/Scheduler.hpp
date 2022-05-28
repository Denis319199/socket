// No copyright

#ifndef FIBER_SCHEDULER_HPP_
#define FIBER_SCHEDULER_HPP_

#include <queue>
#include <utility>

#include <fiber/Routine.hpp>
#include <fiber/StackPool.hpp>

class ExecutionContext;

class Fiber {
 public:
  Fiber();

  ~Fiber();

  explicit Fiber(Routine &&routine);

  void *GetStackPointer() { return &rsp_; }

 private:
  static void Trampoline(Fiber *fiber);

  Routine routine_;
  void *rsp_;
};

namespace details {

extern "C" void SetTrampoline(void *rsp, void (*small_trampoline)(),
                              void (*trampoline)(Fiber *), Fiber *fiber);
extern "C" void SwitchContext(void *to, void *from,
                              void (*schedule_func)(Fiber *), Fiber *fiber);
extern "C" void SwitchContextFinally(void *to);
extern "C" void SmallTrampoline();

}  // namespace details

class ExecutionContext {
 public:
  explicit ExecutionContext(Fiber *fiber);

  void SwitchTo(Fiber *fiber);

  void SwitchToFinally(Fiber *fiber);

 private:
  Fiber *executing_fiber_;
};

class Scheduler {
 public:
  template<class T, class... Args>
  static void Schedule(T &&routine, Args &&...args) {
    queue_.push(new Fiber{
        Routine{std::forward<T>(routine), std::forward<Args>(args)...}});
  }

  static void Yield();

 private:
  static void ScheduleFiber(Fiber *fiber);

  static void Return(Fiber *fiber);

  static Fiber *GetNextFiber();

  friend ExecutionContext;
  friend Fiber;

  static inline Fiber main_fiber_{};
  static inline ExecutionContext execution_context_{&main_fiber_};
  static inline std::queue<Fiber *> queue_{};
};

#endif  // FIBER_SCHEDULER_HPP_
