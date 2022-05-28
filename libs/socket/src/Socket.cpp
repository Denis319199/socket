// No copyright

#include <socket/Socket.hpp>

/****************** SocketCreationException Implementation ******************/

const char *SocketCreationException::what() const noexcept {
  return "Error occurred during socket creation\n";
}

/****************** SocketConnectionException Implementation ******************/

const char *SocketConnectionException::what() const noexcept {
  return "Error occurred in socket connection\n";
}

/******************** Socket Implementation ********************/

#if defined(_WIN32) || defined(_WIN64)
Socket::Socket(const char *host, unsigned short port)
    : socket_{INVALID_SOCKET}, is_shutdown_{} {
  socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (socket_ == INVALID_SOCKET) {
    throw SocketCreationException{};
  }

  addrinfo hints{};
  addrinfo *result{};

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  auto addr_info_res{
      getaddrinfo(host, std::to_string(port).c_str(), &hints, &result)};
  if (addr_info_res != 0) {
    throw SocketCreationException{};
  }

  auto ptr{result};
  for (; ptr != nullptr; ptr = ptr->ai_next) {
    auto connect_res{connect(socket_, ptr->ai_addr, ptr->ai_addrlen)};
    if (connect_res != SOCKET_ERROR) {
      break;
    }
  }

  freeaddrinfo(result);

  if (ptr == nullptr) {
    closesocket(socket_);

    throw SocketConnectionException{};
  }
}

Socket::Socket(SOCKET socket) : socket_{socket}, is_shutdown_{} {}

Socket::~Socket() {
  if (socket_ != INVALID_SOCKET) {
    closesocket(socket_);
  }
}

Socket::Socket(Socket &&other) noexcept
    : socket_{other.socket_}, is_shutdown_{other.is_shutdown_} {
  other.is_shutdown_ = true;
  other.socket_ = INVALID_SOCKET;
}

int Socket::Read(char *buffer, int size) {
  auto sym_num{recv(socket_, buffer, size, 0)};
  if (sym_num < 0) {
    throw SocketConnectionException{};
  }

  return sym_num;
}

void Socket::Write(const char *buffer, int size) {
  int sym_sent{};

  while (sym_sent != size) {
    auto send_res{send(socket_, buffer + sym_sent, size, 0)};
    if (send_res == SOCKET_ERROR) {
      throw SocketConnectionException{};
    }
    sym_sent += send_res;
  }
}

void Socket::Shutdown() {
  if (!is_shutdown_) {
    auto shutdown_res{shutdown(socket_, SD_SEND)};
    if (shutdown_res == SOCKET_ERROR) {
      throw SocketConnectionException{};
    }

    is_shutdown_ = true;
  }
}

Socket::SocketShutdownHook::SocketShutdownHook() noexcept {
  WSADATA wsa_data;
  auto res{WSAStartup(MAKEWORD(2, 2), &wsa_data)};

  if (res != NO_ERROR) {
    std::cerr << "Could not initialize sockets\n";
    std::terminate();
  }
}

Socket::SocketShutdownHook::~SocketShutdownHook() { WSACleanup(); }

#elif defined(__unix__)

Socket::Socket(const char *host, unsigned short port)
    : socket_{-1}, is_shutdown_{} {
  socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (socket_ == -1) {
    throw SocketCreationException{};
  }

  addrinfo hints{};
  addrinfo *result{};

  std::memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  auto addr_info_res{
          getaddrinfo(host, std::to_string(port).c_str(), &hints, &result)};
  if (addr_info_res != 0) {
    throw SocketCreationException{};
  }

  auto ptr{result};
  for (; ptr != nullptr; ptr = ptr->ai_next) {
    auto connect_res{connect(socket_, ptr->ai_addr, ptr->ai_addrlen)};
    if (connect_res != -1) {
      break;
    }
  }

  freeaddrinfo(result);

  if (ptr == nullptr) {
    close(socket_);

    throw SocketConnectionException{};
  }
}

Socket::Socket(int socket) : socket_{socket}, is_shutdown_{} {}

Socket::~Socket() {
  if (socket_ >= 0) {
    close(socket_);
  }
}

Socket::Socket(Socket &&other) noexcept
    : socket_{other.socket_}, is_shutdown_{other.is_shutdown_} {
  other.is_shutdown_ = true;
  other.socket_ = -1;
}

ssize_t Socket::Read(char *buffer, int size) {
  auto sym_num{recv(socket_, buffer, size, 0)};
  if (sym_num < 0) {
    throw SocketConnectionException{};
  }

  return sym_num;
}

void Socket::Write(char *buffer, int size) {
  int sym_sent{};

  while (sym_sent != size) {
    auto send_res{send(socket_, buffer, size, 0)};
    if (send_res < 0) {
      throw SocketConnectionException{};
    }
    sym_sent += send_res;
  }
}

void Socket::Shutdown() {
  if (!is_shutdown_) {
    auto shutdown_res{shutdown(socket_, SHUT_WR)};
    if (shutdown_res < 0) {
      throw SocketConnectionException{};
    }

    is_shutdown_ = true;
  }
}

#endif

/******************** ServerSocket Implementation ********************/

#if defined(_WIN32) || defined(_WIN64)

ServerSocket::ServerSocket(unsigned short port) : socket_{INVALID_SOCKET} {
  socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (socket_ == INVALID_SOCKET) {
    throw SocketCreationException{};
  }

  addrinfo *result{};
  addrinfo hints{};

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  auto addr_info_res{
      getaddrinfo(nullptr, std::to_string(port).c_str(), &hints, &result)};
  if (addr_info_res != 0) {
    throw SocketCreationException{};
  }

  auto ptr{result};
  for (; ptr != nullptr; ptr = ptr->ai_next) {
    auto bind_res
        {bind(socket_, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen))};
    if (bind_res != SOCKET_ERROR) {
      break;
    }
  }

  freeaddrinfo(result);

  if (ptr == nullptr) {
    closesocket(socket_);
    throw SocketConnectionException{};
  }

  if (listen(socket_, SOMAXCONN) == SOCKET_ERROR) {
    closesocket(socket_);
    throw SocketConnectionException{};
  }
}

ServerSocket::~ServerSocket() {
  if (socket_ != INVALID_SOCKET) {
    closesocket(socket_);
  }
}

Socket ServerSocket::Accept() {
  auto client_socket_ = accept(socket_, nullptr, nullptr);
  if (client_socket_ == INVALID_SOCKET) {
    throw SocketConnectionException{};
  }

  return Socket{client_socket_};
}

ServerSocket::ServerSocket(ServerSocket &&other) noexcept
    : socket_{other.socket_} {
  other.socket_ = INVALID_SOCKET;
}

#elif defined(__unix__)

ServerSocket::ServerSocket(unsigned short port) : socket_{-1} {
  socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (socket_ == -1) {
    throw SocketCreationException{};
  }

  addrinfo hints{};
  addrinfo *result{};

  std::memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  auto addr_info_res{
          getaddrinfo(nullptr, std::to_string(port).c_str(), &hints, &result)};
  if (addr_info_res != 0) {
    throw SocketCreationException{};
  }

  auto ptr{result};
  for (; ptr != nullptr; ptr = ptr->ai_next) {
    auto bind_res
            {bind(socket_, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen))};
    if (bind_res != -1) {
      break;
    }
  }

  freeaddrinfo(result);

  if (ptr == nullptr) {
    close(socket_);
    throw SocketConnectionException{};
  }

  if (listen(socket_, SOMAXCONN) == -1) {
    close(socket_);
    throw SocketConnectionException{};
  }
}

ServerSocket::~ServerSocket() {
  if (socket_ >= 0) {
    close(socket_);
  }
}

Socket ServerSocket::Accept() {
  auto client_socket_ = accept(socket_, nullptr, nullptr);
  if (client_socket_ < 0) {
    throw SocketConnectionException{};
  }

  return Socket{client_socket_};
}

ServerSocket::ServerSocket(ServerSocket &&other) noexcept
    : socket_{other.socket_} {
  other.socket_ = -1;
}

#endif
