#include "ITcp.h"
#include "TcpEndpoint.h"

#include <sys/socket.h>
#include <sys/un.h>

#include <string>

class TcpClient : public TcpEndpoint, public ITcpClient {
public:

  TcpClient() 
  : TcpEndpoint() {
    // Start the thread.
    worker_thread_ = std::thread{&TcpClient::handleConnections, this};
  };

  ~TcpClient() {
    cout << "TcpClient going away.  Quit now is true." << endl;
    quit_ = true;
  };

  void recvData(void* data, size_t nbytes) {
    TcpEndpoint::recvData(data, nbytes);
  };

  bool isConnected() { return connected_; }

  void handleConnections() {

    while(!quit_) {
      char socket_name[108]; // 108 sun_path length max

      // Use a local(same machine), for IPC, socket (i.e. AF_UNIX or AF_LOCAL);
      data_socket_ = socket(AF_UNIX, SOCK_STREAM, 0);
      if (data_socket_ < 0) {
        cout << "Failed to create client socket: " << strerror(errno) << endl;
        return;
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

      cout << "Connecting ..." << endl;
      if (connect(data_socket_, (const struct sockaddr *)&server_addr_, sizeof(struct sockaddr_un)) == 0) {
        // Connected right away.
        connected_ = true;
      } else if (pollOnError(EINPROGRESS, data_socket_, POLLOUT)) { 
          // The socket is nonblocking and the connection could  not be 
          // completed immediately but on polling, we got a POLLOUT meaning
          // indicating that writing is now possible
          connected_ = true;
      } else {
        cout << "Connection to server failed: " << strerror(errno) << endl;
      }

      if (connected_) {
        // Check for disconnection requests.
        monitor_connection(this);
        connected_ = false;
      }
      cout << "client closing socket: " << data_socket_ << endl;
      close(data_socket_);
    }

    cout << "Client handle_connections thread terminated." << endl;
  }
};

std::unique_ptr<ITcpClient> ITcpClient::Create() {
  return std::make_unique<TcpClient>();
};

