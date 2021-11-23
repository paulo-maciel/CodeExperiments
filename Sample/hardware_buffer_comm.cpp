#include "hardware_buffer_comm.h"

#define ML_USE_ANDROID_HARDWARE_BUFFER
#ifdef ML_USE_ANDROID_HARDWARE_BUFFER
//#include <ml/gfx/hardware_buffer.h>

#include <cstring>
#include <iostream>
#include <poll.h>
#include <thread>
#include <unistd.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

using namespace std;

#ifdef USING_HW_SOCKET

constexpr char EOT = '$';

int AHardwareBuffer_sendHandleToUnixSocket(AHardwareBuffer* buffer, int data_socket) {
  // int i = 1;
  // int ret = setsockopt( data_socket, IPPROTO_TCP, TCP_NODELAY, (void *)&i, sizeof(i));
  // if (ret < 0) {
  //   cout << "AHardwareBuffer_sendHandleToUnixSocket: Call to setsockopt failed:" << strerror(errno) << endl;
  // }
  size_t bytesSent = send(data_socket, buffer->str.data(), buffer->str.length(), MSG_DONTWAIT);
  cout << buffer->str << " " << endl;
  cout << "     Sent: " << bytesSent << " bytes" << endl;
  return bytesSent;
}

int AHardwareBuffer_recvHandleFromUnixSocket(int data_socket, AHardwareBuffer **buffer) {
  size_t bytesRecvd = recv(data_socket, const_cast<char *>((*buffer)->str.data()), (*buffer)->str.length(), 0);
  cout << (*buffer)->str << " " << endl;
  cout << "     Received: " << bytesRecvd << " bytes" << endl;

  // Check for EOT
  if ((*buffer)->str[0] == EOT) {
    return EOT;
  }
  return bytesRecvd;
}

HardwareBufferComm::HardwareBufferComm()
  : client_set_(false)
  , server_set_(false) {
}

HardwareBufferComm ::~HardwareBufferComm() {
  close(data_socket_);
  data_socket_ = 0;

  if (server_set_) {
    close(ssocket_fd_);
    ssocket_fd_ = 0;
  }

  server_set_ = false;
  client_set_ = false;
}

bool HardwareBufferComm::setupClient() const {
  char socket_name[108]; // 108 sun_path length max

  if (client_set_) {
    return true;
  }
  // Use a local, for IPC, socket (i.e. AF_UNIX);
  data_socket_ = socket(AF_UNIX, SOCK_STREAM, 0);
  if (data_socket_ < 0) {
    cout << "Failed to create client socket: " << strerror(errno) << endl;
    return false;
  }

  // NDK needs abstract namespace by leading with '\0'
  // http://www.toptip.ca/2013/01/unix-domain-socket-with-abstract-socket.html?m=1
  // Note you don't need to unlink() the socket then.
  memcpy((void *)&socket_name[0], "\0", 1);
  strcpy(&socket_name[1], SOCKET_NAME);

  // clear for safety
  memset(&server_addr_, 0, sizeof(struct sockaddr_un));
  server_addr_.sun_family = AF_UNIX;                                              // Unix Domain/local instead of AF_INET IP domain.
  strncpy(server_addr_.sun_path, socket_name, sizeof(server_addr_.sun_path) - 1); // 108 char max

  // Assuming only one init connection for demo.
  int trials = 10;
  while (true) {
    cout << "Client attempting to connect ..." << endl;
    int ret = connect(data_socket_, (const struct sockaddr *)&server_addr_, sizeof(struct sockaddr_un));
    if (ret < 0) {
      if (errno == EISCONN) {
        break;
      }
      cout << "Client attempt to connect failed: " << strerror(errno) << endl;
      --trials;
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      if (!trials) {
        cout << "Client failed to connect: " << strerror(errno) << " Giving up." << endl;
        return false;
      }
    }
  }

  client_set_ = true;
  cout << "Client connected!" << endl;
  return true;
}

// Server to get socket data with information of SharedMem's file descriptor
bool HardwareBufferComm::setupServer() {
  int  ret;
  char socket_name[108]; // 108 sun_path length max

  if (server_set_) {
    return true;
  }

  cout << "Start server setup" << endl;

  // AF_UNIX for domain unix IPC and SOCK_STREAM since it works for the example
  int ssocket_fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
  if (ssocket_fd_ < 0) {
    cout << "Failed to create server socket: " << strerror(errno) << endl;
    return false;
  }
  cout << "Socket created" << endl;

  // NDK needs abstract namespace by leading with '\0'
  // http://www.toptip.ca/2013/01/unix-domain-socket-with-abstract-socket.html?m=1
  // Note you don't need to unlink() the socket then.
  memcpy(&socket_name[0], "\0", 1);
  strcpy(&socket_name[1], SOCKET_NAME);

  // clear for safety.
  memset(&server_addr_, 0, sizeof(struct sockaddr_un));
  server_addr_.sun_family = AF_UNIX;                                              // Unix Domain instead of AF_INET IP domain
  strncpy(server_addr_.sun_path, socket_name, sizeof(server_addr_.sun_path) - 1); // 108 char max

  // Use the global namespace to disambiguate from std::bind.
  ret = ::bind(ssocket_fd_, (const struct sockaddr *)&server_addr_, (unsigned int)sizeof(struct sockaddr_un));
  if (ret < 0) {
    cout << "Failed to bind server socket: " << strerror(errno) << endl;
    return false;
  }
  cout << "Bind success" << endl;

  // Open 8 back buffers for this demo
  ret = listen(ssocket_fd_, 8);
  if (ret < 0) {
    cout << "Server failed to listen on socket: " << strerror(errno) << endl;
    return false;
  }
  cout << "Socket listening for packages.  Polling started." << endl;

  // Wait for incoming connection.
  int trials = 10;
  while (true) {
    // assume there is data to read?
    short revents;
    // POLLIN means that there are events to read.
    pollfd state = {ssocket_fd_, POLLIN, revents};
    
    // wait 1000 ms.
    cout << "     poll .....";
    auto ret = poll(&state, 1, 1000);
    // Continue if time mout expired.
    if (ret == 0) {
      continue;
    } 

    if (ret > 0 || revents == POLLIN) {
      cout << endl << "Ready to accept!!!!" << endl;
      break;
    } else {
      cout << "poll failed: " << strerror(errno) << "Will try to connect again." << endl;
      --trials;
      if (!trials) {
        cout << "Poll failed. Giving up." << endl;
        return false;
      }
    }
  }

  data_socket_ = accept4(ssocket_fd_, NULL, NULL, SOCK_NONBLOCK);
  if (data_socket_ < 0) {
    cout << "Server failed to accept: " << strerror(errno) << endl;
    return false;
  }
  cout << "Server connected!" << endl;

  server_set_ = true;
  return true;
}

int HardwareBufferComm::sendHandle(AHardwareBuffer *buffer) const {
  if (!client_set_) {
    cout << "HardwareBufferComm::sendHandle: Client has not beeing set." << endl;
    return -1;
  }

  int ret = AHardwareBuffer_sendHandleToUnixSocket(buffer, data_socket_);
  if (ret < 0) {
    cout << "HardwareBufferComm::sendHandle: Call to AHardwareBuffer_sendHandleToUnixSocket failed." << endl;
    return -1;
  }
  return ret;
}

int HardwareBufferComm::recvHandle(AHardwareBuffer *buffer) {
  if (!server_set_) {
    cout << "HardwareBufferComm::recvHandle: Server has not beeing set." << endl;
    return -1;
  }

  int ret = AHardwareBuffer_recvHandleFromUnixSocket(data_socket_, &buffer);
  if (ret == EOT || ret == 0) {
    return -1;
  }
  if (ret < 0) {
    cout << "HardwareBufferComm::recvHandle: Call to AHardwareBuffer_recvHandleFromUnixSocket failed." << endl;
    return -1;
  }

  return ret;
}

#endif
#endif