// No copyright

#ifndef SOCKET_MUTEX_HPP_
#define SOCKET_MUTEX_HPP_

#include <exception>

class MutexCreationException : public std::exception {
  [[nodiscard]] const char *what() const noexcept override {
    return "Error occurred during mutex creation";
  }
};

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>

class Mutex {
 public:
  Mutex() {
    mutex_ = CreateMutex(nullptr, false, 0);
    if (mutex_ == nullptr) { throw MutexCreationException{}; }
  }

  ~Mutex() { CloseHandle(mutex_); }

  void Lock() { WaitForSingleObject(mutex_, INFINITE); }

  void Unlock() { ReleaseMutex(mutex_); }

 private:
  HANDLE mutex_;
};

#elif defined(__unix__)

class Mutex {
 public:
  Mutex() {
    if (pthread_mutex_init(&mutex_, nullptr) != 0) {
      throw MutexCreationException{};
    }
  }

  ~Mutex() { pthread_mutex_destroy(&mutex_); }

  void Lock() { pthread_mutex_lock(&mutex_); }

  void Unlock() { pthread_mutex_unlock(&mutex_); }

 private:
  pthread_mutex_t mutex_;
};

#endif

class LockGuard {
 public:
  explicit LockGuard(Mutex &mutex) : mutex_{mutex} { mutex_.Lock(); }

  ~LockGuard() { mutex_.Unlock(); }

 private:
  Mutex &mutex_;
};

#endif  // SOCKET_MUTEX_HPP_
