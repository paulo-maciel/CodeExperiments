#pragma once

#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>

#include <thread>
#include <unistd.h>

#include <iostream>
#include <vector>
using namespace std;

class TcpEndpoint {
public:
  // Can be anything.
  const char *SOCKET_NAME = "sharedSocketName";

  TcpEndpoint()
  : quit_(false)
  , connected_(false)
  , disconnect_(false) {};

  ~TcpEndpoint() {
    worker_thread_.join();
  };

  bool isConnected() { return connected_;}

  bool pollOnError(int err, int sockfd, short pollmask) {
    if (errno == err) {
      pollfd state = {sockfd, pollmask};
      const int infinte_timeout = -1;
      if (poll(&state, 1, infinte_timeout) < 0) {
        cout << "poll failed: " << errno << " " << strerror(errno) << endl;
      } else if (state.revents & pollmask) {
        // Condition specified in the pollmask was set.
        return true;
      } else {
        cout << "unhandled revents: " << state.revents << endl;
      }
    }

    return false;
  }

  void recvData(void* data, size_t nbytes) {
    myread(data, nbytes);
  }

  void sendData(void* data, size_t nbytes) {
    mywrite(data, nbytes);
  }

  template <class T>
  void monitor_connection(T* handler) {
    vector<char> buffer(256, '\0');
    while(!quit_ && !disconnect_) {
      if (myread(buffer.data(), sizeof(buffer)) != sizeof(buffer)) {
        break;
      }
      cout << "monitor_connection: read: " << buffer.data() << endl;      
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      cout << "Connected_worker wake..." << endl;
      (void)handler;
      //handler->onMsg(msg);
    }
  }

  template <typename T> ssize_t mywrite(const T& v) {
    return mywrite(&v, sizeof(v));
  }

  ssize_t mywrite(const void* buf, size_t nbytes) {
    size_t remain = nbytes;
    for (auto p = static_cast<const char*> (buf); remain;) {
      auto n = write(data_socket_, p, remain);
      if (n > 0) {
        p += n;
        remain -= size_t(n);
        cout << "sent: " << n << "/" << remain << "bytes remanining/total: " << (nbytes - remain) << "/" <<  nbytes << endl;
        continue;
      }
      if (n == 0) {
        cout << "total sent: " << (nbytes - remain) << "/" << nbytes << " Failed: other side closed connection\n" << endl;
        connected_ = false;
        return 0;
      }
      if (pollOnError(EAGAIN, data_socket_, POLLOUT)) {
        continue;
      }
      if (quit_) {
        return 0;
      }
      cout << "sent: " << remain << " bytes.  Failed: " << errno << " " << strerror(errno) << endl;
      return -1;
    }
    return ssize_t(nbytes - remain);
  }

  template <typename T> ssize_t myread(T& v) {
    return myread(&v, sizeof(v));
  }
  ssize_t myread(void* b, size_t nbytes) {
    size_t remain = nbytes;
    for (auto p = static_cast<char*>(b); remain;) {
      auto n = read(data_socket_, p, remain);
      if (n > 0) {
        p += n;
        remain -= size_t(n);
        cout << "recvd: " << n << "/" << remain << "bytes remanining/total: " << (nbytes - remain) << "/" <<  nbytes << endl;
        continue;
      }
      ssize_t rc = 0;
      if (n == 0) {
        cout << "total recvd: " << (nbytes - remain) << "/" << nbytes << " Failed: other side closed connection\n" << endl;
        connected_ = false;
      } else if (pollOnError(EAGAIN, data_socket_, POLLIN)) {
        continue;
      } else if (!quit_) {
        cout << "recvd: " << remain << " bytes.  Failed: " << errno << " " << strerror(errno) << endl;
        rc = -1;
      }
      return rc;
    }
    return ssize_t(nbytes);
  }

protected:
  // Reference to the worker thread.
  std::thread worker_thread_;
  bool quit_;
  bool connected_;
  bool disconnect_;

  // Socket related.
  mutable int  data_socket_;

  // Server side
  mutable struct sockaddr_un server_addr_;
};

