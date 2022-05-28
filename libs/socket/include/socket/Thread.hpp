// No copyright

#ifndef SOCKET_THREAD_HPP_
#define SOCKET_THREAD_HPP_

#include <exception>
#include <tuple>
#include <utility>

class ThreadCreationException : public std::exception {
  [[nodiscard]] const char *what() const noexcept override {
    return "Error occurred during socket creation";
  }
};

namespace util {

template<std::size_t... Seq>
struct IndexSequence {};

template<std::size_t num, std::size_t... Seq>
struct MakeIndexSequence : MakeIndexSequence<num - 1, num - 1, Seq...> {};

template<std::size_t... Seq>
struct MakeIndexSequence<0, Seq...> : IndexSequence<Seq...> {};

template<class... Args>
using IndexSequenceFor = MakeIndexSequence<sizeof...(Args)>;

}  // namespace util

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>

class Thread {
 public:
  template<class... Args>
  explicit Thread(void (*routine)(Args...), Args &&...args)
      : routine_wrapper_{
          new RoutineWrapper<Args...>{routine, std::forward<Args>(args)...}} {
    thread_ = CreateThread(nullptr, 0, StartRoutine, routine_wrapper_, 0,
                           &thread_id_);
    if (thread_ == nullptr) { throw ThreadCreationException{}; }
  }

  void Join() {
    WaitForSingleObject(thread_, INFINITE);
    CloseHandle(thread_);
  }

  void Detach() { CloseHandle(thread_); }

 private:
  static DWORD StartRoutine(LPVOID routine_wrapper) {
    auto routine_wrapper_casted{
        static_cast<RoutineWrapperBase *>(routine_wrapper)};
    routine_wrapper_casted->Run();
    return 0;
  }

  class RoutineWrapperBase {
   public:
    RoutineWrapperBase() = default;

    virtual ~RoutineWrapperBase() = default;

    virtual void Run() = 0;
  };

  template<class... Args>
  class RoutineWrapper : public RoutineWrapperBase {
   public:
    explicit RoutineWrapper(void (*routine)(Args...), Args &&...args)
        : routine_{routine},
          args_{std::forward<Args>(args)...} {}

   private:
    void Run() final { RunImpl(util::IndexSequenceFor<Args...>{}); }

    template<std::size_t... Seq>
    void RunImpl(util::IndexSequence<Seq...> seq) {
      routine_(std::forward<Args>(std::get<Seq>(args_))...);
    }

    void (*routine_)(Args...);
    std::tuple<Args...> args_;
  };

  RoutineWrapperBase *routine_wrapper_;
  HANDLE thread_;
  DWORD thread_id_;
};

#elif defined(__unix__)

#include <pthread.h>

class Thread {
 public:
  template<class... Args>
  explicit Thread(void (*routine)(Args...), Args &&...args)
      : routine_wrapper_{
          new RoutineWrapper<Args...>{routine, std::forward<Args>(args)...}} {
    if (pthread_create(&thread_, nullptr, StartRoutine, routine_wrapper_)
        != 0) {
      std::terminate();
    }
  }

  void Join() { pthread_join(thread_, nullptr); }

  void Detach() { pthread_detach(thread_); }

 private:
  static void *StartRoutine(void *routine_wrapper) {
    auto routine_wrapper_casted{
        static_cast<RoutineWrapperBase *>(routine_wrapper)};
    routine_wrapper_casted->Run();
    return nullptr;
  }

  class RoutineWrapperBase {
   public:
    RoutineWrapperBase() {}

    virtual ~RoutineWrapperBase() = default;

    virtual void Run() = 0;
  };

  template<class... Args>
  class RoutineWrapper : public RoutineWrapperBase {
   public:
    explicit RoutineWrapper(void (*routine)(Args...), Args &&...args)
        : routine_{routine},
          args_{std::forward<Args>(args)...} {}

   private:
    void Run() final { RunImpl(util::IndexSequenceFor<Args...>{}); }

   private:
    template<std::size_t... Seq>
    void RunImpl(util::IndexSequence<Seq...> seq) {
      routine_(std::forward<Args>(std::get<Seq>(args_))...);
    }

    void (*routine_)(Args...);
    std::tuple<Args...> args_;
  };

  RoutineWrapperBase *routine_wrapper_;
  pthread_t thread_;
};

#endif

#endif  // SOCKET_THREAD_HPP_
