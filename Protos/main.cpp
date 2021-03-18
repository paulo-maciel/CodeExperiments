#include <DebugGpuAlloc.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>

using namespace std;

// extern const std::unordered_map<GLenum, std::string> DebugGpuAlloc::s_targets;

int main()
{
  DebugGpuAlloc& gpuAlloc = DebugGpuAlloc::Get();
  int n = 3;
  GLenum target = GL_TEXTURE_2D;

  GLuint texs[n];
  gpuAlloc.glGenTextures(n, texs);
  gpuAlloc.glTextureStorage2D(texs[0], 3, GL_RGBA, 10, 10);
  gpuAlloc.glTextureStorage2D(texs[1], 1, GL_RGB, 10, 10);
  gpuAlloc.glTextureStorage2D(texs[2], 1, GL_RG8, 10, 10);

  cout << "Total for target: " << DebugGpuAlloc::s_targets.at(target) << " size:  " << gpuAlloc.getAllocated(getpid()) << endl;
  gpuAlloc.glDeleteTextures(n, texs);
  cout << "Total after delete: " << DebugGpuAlloc::s_targets.at(target) << " size:  " << gpuAlloc.getAllocated(getpid()) << endl;

  n = 4;
  GLuint bufs[n];
  gpuAlloc.glGenBuffers(n, bufs);

  target = GL_ARRAY_BUFFER;
  gpuAlloc.glBindBuffer(target, bufs[0]);
  uint8_t* data = 0;
  GLenum usage;
  gpuAlloc.glBufferData(target, 1024, data, usage);
  cout << "Total for target: " << DebugGpuAlloc::s_targets.at(target) << " size:  " << gpuAlloc.getAllocated(getpid()) << endl;
  gpuAlloc.glBufferData(target, 512, data, usage);

  cout << "Total for target: " << DebugGpuAlloc::s_targets.at(target) << " size:  " << gpuAlloc.getAllocated(getpid()) << endl;
  gpuAlloc.glDeleteBuffers(n, bufs);
  cout << "Total after delete: " << DebugGpuAlloc::s_targets.at(target) << " size:  " <<  gpuAlloc.getAllocated(getpid()) << endl;

  return 1;
}