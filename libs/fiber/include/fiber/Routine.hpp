// No copyright

#ifndef FIBER_ROUTINE_HPP_
#define FIBER_ROUTINE_HPP_

#include <tuple>
#include <utility>

class RoutineWrapperBase {
 public:
  virtual void Run() = 0;

  virtual ~RoutineWrapperBase() {}
};

template<class T, class... Args>
class RoutineWrapper : public RoutineWrapperBase {
 public:
  explicit RoutineWrapper(T &&routine, Args &&...args)
      : routine_{std::forward<T>(routine)},
        args_{std::forward<Args>(args)...} {}

  void Run() override { RunImpl(std::index_sequence_for<Args...>{}); }

 private:
  template<std::size_t... Seq>
  void RunImpl(std::index_sequence<Seq...>) {
    routine_(std::move(std::get<Seq>(args_))...);
  }

  std::remove_reference_t<T> routine_;
  std::tuple<std::remove_reference_t<Args>...> args_;
};

template<class T>
class RoutineWrapper<T> : public RoutineWrapperBase {
 public:
  explicit RoutineWrapper(T &&routine) : routine_{std::forward<T>(routine)} {}

  void Run() override { routine_(); }

  T routine_;
};

class Routine {
 public:
  Routine();

  template<class T, class... Args>
  explicit Routine(T &&routine, Args &&...args)
      : routine_wrapper_{new RoutineWrapper<T, Args...>{
          std::forward<T>(routine), std::forward<Args>(args)...}} {}

  Routine(const Routine &) = delete;
  Routine &operator=(const Routine &) = delete;

  Routine(Routine &&other);

  Routine &operator=(Routine &&other);

  ~Routine();

  void operator()();

  bool IsEmpty();

 private:
  RoutineWrapperBase *routine_wrapper_;
};

#endif  // FIBER_ROUTINE_HPP_
