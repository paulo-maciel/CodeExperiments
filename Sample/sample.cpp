#include "ITcp.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <set>
#include <queue>
#include <deque>
#include <list>
#include <stack>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <tuple>
#include <map>
#include <ostream>
#include <iterator>
#include <thread>

// for pop_heap
#include <algorithm>
#include <sstream>

using namespace std;

int main(int n, char** argv) {

  bool isClient = false;
  if (n != 2 || (*argv[1] != 'c' && *argv[1] != 's')) {
    cout << "Specify 'c' for client and 's' for server." << endl;
    exit(1);
  }
  if (*argv[1] == 's') {

    cout << "Setting up the server..." << endl;
    std::unique_ptr<ITcpServer> pServer = ITcpServer::Create();

    // Wait for the client to connect otherwise this app just exits and
    // the server connection thread is stopped.
    cout << "Will wait now for client to connect ..." << endl;
    bool status = pServer->waitForConnection(std::chrono::milliseconds(10000));
    cout << "Server is connected to client.  Send some messages." << endl;

    string token, toSend = "This is a message to send to server";
    stringstream ss(toSend);
    while (getline(ss, token,' ')) {
      pServer->sendData((char *)token.data(), token.size());
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(20000));
    cout << "Server app will exit now" << endl;

  } else if (*argv[1] == 'c') {
    cout << "Setting up the client..." << endl;
    std::unique_ptr<ITcpClient> pClient = ITcpClient::Create();

    char buffer[256];
    cout << "Client to receive data" << endl;
    pClient->recvData(buffer, 1);
    cout << "Client to exit now" << endl;
  }

}
