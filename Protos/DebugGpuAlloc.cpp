
#include "DebugGpuAlloc.h"

#include <GL/glext.h>

#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <algorithm>

using namespace std;

// TODO: Remove
static GLuint texId = 0;
static GLuint buffId = 0;

DebugGpuAlloc& DebugGpuAlloc::Get() {
  static DebugGpuAlloc debugAlloc;
  return debugAlloc;
}

DebugGpuAlloc::DebugGpuAlloc()
: boundTexture_(-1)
, boundTexTarget_(-1)
, boundBuffer_(-1)
, boundBufTarget_(-1) {
}

DebugGpuAlloc::~DebugGpuAlloc() = default;

void DebugGpuAlloc::glGenTextures(GLsizei n, GLuint* texIds) {
  if (nullptr == texIds) {
    return;
  }
  ::glGenTextures(n, const_cast<GLuint*>(texIds));

  pid_t pid = getpid();
  for(auto i = 0; i < n; ++i) {
    // TODO: Remove
    texIds[i] = ++texId;

    textureIDs_[pid].push_back(texIds[i]);
  }
}

void DebugGpuAlloc::glBindTexture(	GLenum target, GLuint id) {

  auto ids = textureIDs_[getpid()];
  auto it = find(ids.begin(), ids.end(), id);
  if (it == ids.end()) {
    return;
  }

  boundTexTarget_ = target;
  boundTexture_ = id;
  ::glBindTexture(target, id);
}

void DebugGpuAlloc::glTextureStorage2D(GLuint texId, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height) {
  // Compute size.
  size_t allocSize = computeTexSize(levels, internalFormat, width, height);

  // Compute hash.
  size_t hash = getHash(GL_TEXTURE_2D, texId);

  GpuData& gpuData = allocMap_[getpid()][hash];
  gpuData.type_ = GL_TEXTURE_2D;
  gpuData.id_ = texId;
  gpuData.size_ = allocSize;

  // TODO: add this.
  //::glTexStorage2D(texId, levels, internalFormat, width, height);
}

void DebugGpuAlloc::glTexStorage2D(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height) {
  if (boundTexture_ == -1) {
    return;
  }

  if (target == GL_TEXTURE_2D) {

    // Compute size.
    size_t allocSize = computeTexSize(levels, internalFormat, width, height);

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

void DebugGpuAlloc::glDeleteTextures( GLsizei n, const GLuint * texIds) {

  for(auto i = 0; i < n; ++i) {
    auto ids = textureIDs_[getpid()];
    auto it = find(ids.begin(), ids.end(), texIds[i]);
    if (it == ids.end()) {
      continue;
    }

    auto hash = getHash(GL_TEXTURE_2D, texIds[i]);
    auto& idMap = allocMap_.at(getpid());
    idMap.erase(hash);
  }

  ::glDeleteTextures(n, texIds);
}

size_t DebugGpuAlloc::getAllocated(pid_t pid) const {
  const auto& dataMap = allocMap_.at(pid);
  size_t totalSize = 0;
  for(const auto& kvp : dataMap) {
    totalSize += kvp.second.size_;
  }
  return totalSize;
}

size_t DebugGpuAlloc::getSize(pid_t pid, GLenum target, GLuint texId) const {
  const auto& dataMap = allocMap_.at(pid);
  auto hash = getHash(target, texId);

  return dataMap.at(hash).size_;
}

size_t DebugGpuAlloc::computeTexSize(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height) {

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

uint32_t DebugGpuAlloc::getHash(GLenum type, int id) const {
  uint32_t hash = 0;
  hash = static_cast<uint32_t>(type) & (0x0000ffff << 0);
  hash |= (static_cast<uint32_t>(id) << 16) & (0x0000ffff << 16);
  //cout << "Final hash: " << hex << hash << endl;

  return hash;
}

void DebugGpuAlloc::glGenBuffers(GLsizei n, GLuint* bufferIds) {
  if (nullptr == bufferIds) {
    return;
  }
  //::glGenBuffers(n, const_cast<GLuint*>(bufferIds));

  pid_t pid = getpid();
  for(auto i = 0; i < n; ++i) {
    // TODO: Remove
    bufferIds[i] = ++buffId;
    bufferIDs_[pid].push_back(bufferIds[i]);
  }
}

void DebugGpuAlloc::glBindBuffer(GLenum target, GLuint id) {
  auto ids = bufferIDs_[getpid()];
  auto it = find(ids.begin(), ids.end(), id);
  if (it == ids.end()) {
    return;
  }

  boundBufTarget_ = target;
  boundBuffer_ = id;
  // TODO: Uncomment
  //::glBindBuffer(target, texId);
}

// While creating the new storage, any pre-existing data store is deleted.
// The new data store is created with the specified size in bytes and usage.
// With glBufferData (...), you can call that command multiple times on the same
// object and it will orphan the old memory and allocate new storage.
void DebugGpuAlloc::glBufferData(GLenum target, GLsizei size, const void* data, GLenum usage) {
//GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER


}

// With glBufferStorage (...), the buffer's size is set for the lifetime of the object
// (immutable).
void DebugGpuAlloc::glBufferStorage(	GLenum target, GLsizeiptr size, const void * data, GLbitfield flags) {
//GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER


}

void DebugGpuAlloc::glDeleteBuffers(GLsizei n, GLuint* bufIds) {
  for(auto i = 0; i < n; ++i) {
    auto ids = bufferIDs_[getpid()];
    auto it = find(ids.begin(), ids.end(), bufIds[i]);
    if (it == ids.end()) {
      continue;
    }

    auto hash = getHash(GL_TEXTURE_2D, bufIds[i]);
    auto& idMap = allocMap_.at(getpid());
    idMap.erase(hash);
  }

  // TODO: Uncomment.
  //::glDeleteBuffers(n, bufIds);
}