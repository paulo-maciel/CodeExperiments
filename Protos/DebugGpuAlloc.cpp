
#include "DebugGpuAlloc.h"

#include <GL/glext.h>

#include <sys/types.h>
#include <unistd.h>

#include <iostream>
using namespace std;

// TODO: Remove
static GLuint texid = 0;

DebugGpuAlloc& DebugGpuAlloc::Get() {
  static DebugGpuAlloc debugAlloc;
  return debugAlloc;
}

DebugGpuAlloc::DebugGpuAlloc()
: boundTexture_(-1)
, boundTarget_(-1) {
}

DebugGpuAlloc::~DebugGpuAlloc() = default;

void DebugGpuAlloc::glGenTextures(GLsizei n, GLuint* textures) {
  if (nullptr == textures) {
    return;
  }
  ::glGenTextures(n, const_cast<GLuint*>(textures));

  pid_t pid = getpid();
  for(auto i = 0; i < n; ++i) {
    // TODO: Remove
    textures[i] = ++texid;

    textureIDs_[pid].push_back(textures[i]);
  }
}

void DebugGpuAlloc::glTextureStorage2D(GLuint texture, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height) {
  // Compute size.
  size_t allocSize = computeSize(levels, internalFormat, width, height);

  // Compute hash.
  size_t hash = getHash(GL_TEXTURE_2D, texture);

  GpuData& gpuData = allocMap_[getpid()][hash];
  gpuData.type_ = GL_TEXTURE_2D;
  gpuData.id_ = texture;
  gpuData.size_ = allocSize;

  // TODO: add this.
  //::glTexStorage2D(texture, levels, internalFormat, width, height);
}

void DebugGpuAlloc::glTexStorage2D(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height) {
  if (target == GL_TEXTURE_2D && boundTexture_ != -1) {

    // Compute size.
    size_t allocSize = computeSize(levels, internalFormat, width, height);

    // Compute hash.
    size_t hash = getHash(GL_TEXTURE_2D, boundTexture_);

    GpuData& gpuData = allocMap_[getpid()][hash];
    gpuData.type_ = GL_TEXTURE_2D;
    gpuData.id_ = boundTexture_;
    gpuData.size_ = allocSize;

    // TODO: add this.
    //::glTexStorage2D(target, levels, internalFormat, width, height);
  }
}

void DebugGpuAlloc::glBindTexture(	GLenum target, GLuint texture) {
  boundTarget_ = target;
  boundTexture_ = texture;
  ::glBindTexture(target, texture);
}

void DebugGpuAlloc::glDeleteTextures( GLsizei n, const GLuint * textures) {

  // TODO: Remove textures/remove allocated space.
  for(auto i = 0; i < n; ++i) {
    auto hash = getHash(GL_TEXTURE_2D, textures[i]);
    auto& idMap = allocMap_.at(getpid());
    idMap.erase(hash);
  }

  ::glDeleteTextures(n, textures);
}

uint32_t DebugGpuAlloc::getHash(GLenum type, int id) const {
  uint32_t hash = 0;
  hash = static_cast<uint32_t>(type) & (0x0000ffff << 0);
  hash |= (static_cast<uint32_t>(id) << 16) & (0x0000ffff << 16);
  //cout << "Final hash: " << hex << hash << endl;

  return hash;
}

size_t DebugGpuAlloc::computeSize(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height) {

  int numChannels = 0;
  switch(internalFormat) {
    case GL_R8:
    numChannels = 1;
    break;
    case GL_RG8:
    numChannels = 2;
    break;
    case GL_RGB:
    case GL_SRGB8:
    numChannels = 3;
    break;
    case GL_RGBA:
    case GL_SRGB8_ALPHA8:
    numChannels = 4;
    break;
    default: {
      // print error
    }
  }

  size_t allocSize = 0;
  int mipSize = numChannels * width * height;
  for (int i = 0; i < levels; ++i) {
    allocSize += mipSize;
    mipSize /= 4;
  }

  return allocSize;
}

size_t DebugGpuAlloc::getAllocated(pid_t pid) const {
  const auto& dataMap = allocMap_.at(pid);
  size_t totalSize = 0;
  for(const auto& kvp : dataMap) {
    totalSize += kvp.second.size_;
  }
  return totalSize;
}

size_t DebugGpuAlloc::getSize(pid_t pid, GLenum target, GLuint texture) const {
  const auto& dataMap = allocMap_.at(pid);
  auto hash = getHash(target, texture);

  return dataMap.at(hash).size_;
}