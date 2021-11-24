#pragma once

#include <memory>
#include <chrono>
#include <thread>
#include <iostream>

class ITcp {
public:
  virtual bool isConnected() = 0;
  bool waitForConnection(std::chrono::milliseconds waitTime) {
    std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
    while(!this->isConnected()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(300));
      auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(startTime - std::chrono::system_clock::now());
      if (elapsedTime > waitTime) {
        std::cout << "waitForConnection timed out." << std::endl;
        return false;
      }
    }

    return this->isConnected();
  }
};

class ITcpClient : public ITcp {
  public:
    static std::unique_ptr<ITcpClient> Create();
    virtual void recvData(void* data, size_t nbytes) = 0;
};

class ITcpServer : public ITcp {
  public:
    static std::unique_ptr<ITcpServer> Create();
    virtual void sendData(void* data, size_t nbytes) = 0;
};