#include "ITcp.h"
#include "TcpEndpoint.h"

#include <sys/socket.h>
#include <sys/un.h>

#include <string>

class TcpServer : public TcpEndpoint, public ITcpServer {
public:

  TcpServer() {
    // Start the thread.
    worker_thread_ = std::thread{&TcpServer::handleConnections, this};
  };
  ~TcpServer() {};

  void sendData(void* data, size_t nbytes) {
    TcpEndpoint::sendData(data, nbytes);
  }

  bool isConnected() { return connected_; }

  void handleConnections() {
    int  ret;
    char socket_name[108]; // 108 sun_path length max
    cout << "Connection handler started." << endl;

    // AF_UNIX for domain unix IPC and SOCK_STREAM since it works for the example
    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    cout << "socket" << endl;
    if (socket_fd < 0) {
      cout << "Failed to create server socket: " << strerror(errno) << endl;
      return;
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
    ret = ::bind(socket_fd, (const struct sockaddr *)&server_addr_, (unsigned int)sizeof(struct sockaddr_un));
    if (ret < 0) {
      cout << "Failed to bind server socket: " << strerror(errno) << endl;
      return;
    }
    cout << "Bind success" << endl;

    // Open 8 back buffers for this demo
    ret = listen(socket_fd, 8);
    if (ret < 0) {
      cout << "Server failed to listen on socket: " << strerror(errno) << endl;
      return;
    }

    // loop until quit.
    while(!quit_) {
      
      cout << "Server listening...\n" << endl;
      sockaddr_un cli;
      socklen_t len = sizeof(cli);
      data_socket_ = accept4(socket_fd, (sockaddr*)&cli, &len, SOCK_NONBLOCK);
      //data_socket_ = accept4(socket_fd, NULL, NULL, SOCK_NONBLOCK);
      if (data_socket_ < 0) {
        if (pollOnError(EAGAIN, socket_fd, POLLIN)) {
          cout << "...client incoming..." << endl;
          continue;
        }
        if (!quit_) {
          cout << "Quit requested: " << errno << strerror(errno) << endl;
        }
        break;
      }

      connected_ = true;
      cout << "Server connected to client: " << data_socket_ << endl;

      // Check for disconnection requests.
      monitor_connection(this);
      connected_ = false;

      cout << "closing client connection: " << data_socket_ << endl;
      close(data_socket_);
    }

    // Close listening socket.
    close(socket_fd);

    cout << "Server handle_connections thread terminated." << endl;
  }

};

std::unique_ptr<ITcpServer> ITcpServer::Create() {
  return std::make_unique<TcpServer>();
};
