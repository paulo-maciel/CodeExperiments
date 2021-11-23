#pragma once

#include <sys/socket.h>
#include <sys/un.h>

#include <string>

struct AHardwareBuffer {
  std::string str;
};

// Can be anything if using abstract namespace
#define SOCKET_NAME "sharedServerSocket"

#define USING_HW_SOCKET
#ifdef USING_HW_SOCKET
class HardwareBufferComm {
public:
  HardwareBufferComm();
  ~HardwareBufferComm();

  bool setupClient() const;
  bool setupServer();
  int sendHandle(AHardwareBuffer *buffer) const;
  int recvHandle(AHardwareBuffer *buffer);

private:
  mutable bool client_set_;
  mutable bool server_set_;
  mutable int  data_socket_;

  // Server side
  int                        ssocket_fd_;
  mutable struct sockaddr_un server_addr_;
};

#endif
