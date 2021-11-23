#include "hardware_buffer_comm.h"

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

// for pop_heap
#include <algorithm>
#include <sstream>

using namespace std;

int main(int n, char** argv) {
  HardwareBufferComm hbuffer;
  bool isClient = false;
  if (n != 2 || (*argv[1] != 'c' && *argv[1] != 's')) {
    cout << "Specify 'c' for client and 's' for server." << endl;
    exit(1);
  }
  if (*argv[1] == 'c') {
    // We are the client.
    cout << "Setting up the client..." << endl;
    hbuffer.setupClient();
    cout << "Client set!" << endl;
    isClient = true;

    // Send some messages.
    string token, toSend = "This is a message to send to server";
    stringstream ss(toSend);

    AHardwareBuffer buffer;
    while (getline(ss, token,' ')) {
      // cout << token << " ";

      buffer.str = token;
      hbuffer.sendHandle(&buffer);
    }

    // Send an EOT
    buffer.str = "$";
    hbuffer.sendHandle(&buffer);
    cout << endl;

  } else if (*argv[1] == 's') {
    // We are the server
    cout << "Setting up the server..." << endl;
    hbuffer.setupServer();
    cout << "Server set!" << endl;

    // Receive some messages.
    AHardwareBuffer buffer;
    buffer.str = string(1024, '\0');
    while(hbuffer.recvHandle(&buffer) >= 0) {
      buffer.str = string(1024, '\0');
    }
    cout << endl;
  }

}
