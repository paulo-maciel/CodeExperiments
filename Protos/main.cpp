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

int main()
{
  DebugGpuAlloc& gpuAlloc = DebugGpuAlloc::Get();
  GLuint id[3];
  gpuAlloc.glGenTextures(3, id);

  gpuAlloc.glTextureStorage2D(id[0], 3, GL_RGBA, 10, 10);
  gpuAlloc.glTextureStorage2D(id[1], 1, GL_RGB, 10, 10);
  gpuAlloc.glTextureStorage2D(id[2], 1, GL_RG8, 10, 10);

  cout << "Allocated before: " << gpuAlloc.getAllocated(getpid()) << endl;

  gpuAlloc.glDeleteTextures(3, id);

  cout << "Allocated after: " << gpuAlloc.getAllocated(getpid()) << endl;

  return 1;
}