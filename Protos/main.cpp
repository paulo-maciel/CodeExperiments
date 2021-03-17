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

  cout << "Allocated: " << gpuAlloc.getAllocated(getpid()) << endl;

  //cout << "Size of the 3rd id: " << id[2] << " is: " << gpuAlloc.getSize(getpid(), GL_TEXTURE_2D, id[2]) << endl;

  return 1;
}