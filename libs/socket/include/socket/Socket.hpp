// No copyright

#ifndef SOCKET_SOCKET_HPP_
#define SOCKET_SOCKET_HPP_

#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined(__unix__)
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#endif

class SocketCreationException : public std::exception {
 public:
  [[nodiscard]] const char *what() const noexcept override;
};

class SocketConnectionException : public std::exception {
 public:
  [[nodiscard]] const char *what() const noexcept override;
};

class Socket {
 public:
#if defined(_WIN32) || defined(_WIN64)
  using SizeType = int;
#elif defined(__unix__)
  using SizeType = ssize_t;
#endif

  Socket(const char *host, unsigned short port);

  Socket(const Socket &) = delete;

  Socket &operator=(const Socket &) = delete;

  Socket(Socket &&other) noexcept;

  void Shutdown();

#if defined(_WIN32) || defined(_WIN64)
  int Read(char *buffer, int size);
#elif defined(__unix__)
  ssize_t Read(char *buffer, int size);
#endif

  void Write(const char *buffer, int size);

  ~Socket();

 private:
#if defined(_WIN32) || defined(_WIN64)
  class SocketShutdownHook {
   public:
    SocketShutdownHook() noexcept;

    ~SocketShutdownHook();
  };
#endif

#if defined(_WIN32) || defined(_WIN64)
  explicit Socket(SOCKET socket);
#elif defined(__unix__)
  explicit Socket(int socket);
#endif

  friend class ServerSocket;

#if defined(_WIN32) || defined(_WIN64)
  SOCKET socket_;
#elif defined(__unix__)
  int socket_;
#endif

  bool is_shutdown_;

#if defined(_WIN32) || defined(_WIN64)
  inline static SocketShutdownHook socketShutdownHook{};
#endif
};

class ServerSocket {
 public:
  explicit ServerSocket(unsigned short port);

  ServerSocket(const ServerSocket &) = delete;

  ServerSocket &operator=(const ServerSocket &) = delete;

  ServerSocket(ServerSocket &&other) noexcept;

  ~ServerSocket();

  Socket Accept();

 private:
#if defined(_WIN32) || defined(_WIN64)
  SOCKET socket_;
#elif defined(__unix__)
  int socket_;
#endif
};

#endif  // SOCKET_SOCKET_HPP_
