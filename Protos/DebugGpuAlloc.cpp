
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

// static
const std::array<GLenum, 10> DebugGpuAlloc::s_texTargets = {
  GL_TEXTURE_2D,
  GL_TEXTURE_3D,
  GL_TEXTURE_2D_ARRAY,
  GL_TEXTURE_CUBE_MAP,
  GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

// static
const std::array<GLenum, 5> DebugGpuAlloc::s_bufTargets = {
  GL_ARRAY_BUFFER,
  GL_ELEMENT_ARRAY_BUFFER,
  GL_PIXEL_UNPACK_BUFFER,
  GL_TEXTURE_BUFFER,
  GL_UNIFORM_BUFFER
};

// static
const std::unordered_map<GLenum, std::string> DebugGpuAlloc::s_targets = {
  {DebugGpuAlloc::s_texTargets[0], "GL_TEXTURE_2D"},
  {DebugGpuAlloc::s_bufTargets[0], "GL_ARRAY_BUFFER"},
  {DebugGpuAlloc::s_bufTargets[1], "GL_ELEMENT_ARRAY_BUFFER"},
  {DebugGpuAlloc::s_bufTargets[2], "GL_PIXEL_UNPACK_BUFFER"},
  {DebugGpuAlloc::s_bufTargets[3], "GL_TEXTURE_BUFFER"},
  {DebugGpuAlloc::s_bufTargets[4], "GL_UNIFORM_BUFFER"}
};

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
  }
}

void DebugGpuAlloc::glBindTexture(	GLenum target, GLuint id) {
  boundTexTarget_ = target;
  boundTexture_ = id;
  ::glBindTexture(target, id);
  // TODO: tex image 2D
}

void DebugGpuAlloc::glTexImage2D(	GLenum target, GLint level, GLint internalFormat,
                                  GLsizei width, GLsizei height,
                                  GLint border, GLenum format,
                                  GLenum type, const void * data) {
  if (boundTexture_ == -1) {
    return;
  }

  // Log the memory.
  size_t allocSize = computeTexSize(1, internalFormat, width, height);

  // Compute hash.
  size_t hash = getHash(GL_TEXTURE_2D, boundTexture_);

  GpuData& gpuData = allocMap_[getpid()][hash];
  gpuData.target_ = GL_TEXTURE_2D;
  gpuData.id_ = boundTexture_;
  gpuData.size_ = allocSize;

  ::glTexImage2D(target, level, internalFormat, width, height, border, format, type, data);
}

void DebugGpuAlloc::glTextureStorage2D(GLuint texId, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height) {
  // Compute size.
  size_t allocSize = computeTexSize(levels, internalFormat, width, height);

  // Compute hash.
  size_t hash = getHash(GL_TEXTURE_2D, texId);

  GpuData& gpuData = allocMap_[getpid()][hash];
  gpuData.target_ = GL_TEXTURE_2D;
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
    size_t hash = getHash(target, boundTexture_);

    GpuData& gpuData = allocMap_[getpid()][hash];
    gpuData.target_ = GL_TEXTURE_2D;
    gpuData.id_ = boundTexture_;
    gpuData.size_ = allocSize;

    // TODO: add this.
    //::glTexStorage2D(target, levels, internalFormat, width, height);
  }
}

void DebugGpuAlloc::glDeleteTextures( GLsizei n, const GLuint * texIds) {

  auto& idMap = allocMap_.at(getpid());
  for(auto i = 0; i < n; ++i) {
    // Loop thru all the buffer targets and see if we can find the target,id pair.
    for (auto texTargets : DebugGpuAlloc::s_texTargets) {
      auto hash = getHash(texTargets, texIds[i]);
      if (idMap.count(hash)) {
        idMap.erase(hash);
      }
    }
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
  // TODO: add
  //::glGenBuffers(n, const_cast<GLuint*>(bufferIds));

  // TODO: remove
  pid_t pid = getpid();
  for(auto i = 0; i < n; ++i) {
    // TODO: Remove
    bufferIds[i] = ++buffId;
  }
}

void DebugGpuAlloc::glBindBuffer(GLenum target, GLuint id) {
  boundBufTarget_ = target;
  boundBuffer_ = id;

  // TODO: Uncomment
  //::glBindBuffer(target, texId);
}

// While creating the new storage, any pre-existing data store is deleted.
// The new data store is created with the specified size in bytes and usage.
// With glBufferData (...), you can call that command multiple times on the same
// object and it will orphan the old memory and allocate new storage.
void DebugGpuAlloc::glBufferData(GLenum target, GLsizei allocSize, const void* data, GLenum usage) {
  if (addBuffer(target, allocSize)) {
    // TODO: add this.
    (void)data; (void)usage;
    //::glBufferData(target, allocSize, data, usage);
  }
}

// With glBufferStorage (...), the buffer's size is set for the lifetime of the object
// (immutable).
void DebugGpuAlloc::glBufferStorage(GLenum target, GLsizeiptr allocSize, const void * data, GLbitfield flags) {

  if (addBuffer(target, allocSize)) {
    // TODO: add this.
    (void)data; (void)flags;
    //::glBufferStorage(target, allocSize, data, flags);
  }
}

bool DebugGpuAlloc::addBuffer(GLenum target, GLsizei allocSize) {
  if (target != GL_ARRAY_BUFFER &&
      target != GL_ELEMENT_ARRAY_BUFFER &&
      target != GL_PIXEL_UNPACK_BUFFER &&
      target != GL_TEXTURE_BUFFER &&
      target != GL_UNIFORM_BUFFER) {
    cout << "Target: " << target << " not supported." << endl;
    return false;
  }

  if (boundBuffer_ == -1) {
    cout << "Buffer not bound" << endl;
    return false;
  }

  // Compute hash.
  size_t hash = getHash(target, boundBuffer_);
  bool previousAlloc = allocMap_[getpid()].count(hash) != 0 ? true : false;
  if (previousAlloc) {
    // Remove the logged memory.
    cout << "glBufferData: orphaning previously loaded buffer of size: " << allocMap_[getpid()][hash].size_ << ". Driver should reclaim this memory." << endl;
    allocMap_[getpid()].erase(hash);
  }

  GpuData& gpuData = allocMap_[getpid()][hash];
  gpuData.target_ = target;
  gpuData.id_ = boundBuffer_;
  gpuData.size_ = allocSize;

  return true;
}

void DebugGpuAlloc::glDeleteBuffers(GLsizei n, GLuint* bufIds) {
  auto& idMap = allocMap_.at(getpid());
  for(auto i = 0; i < n; ++i) {
    // Loop thru all the buffer targets and see if we can find the target,id pair.
    for (auto bufTarget : DebugGpuAlloc::s_bufTargets) {
      auto hash = getHash(bufTarget, bufIds[i]);
      if (idMap.count(hash)) {
        idMap.erase(hash);
      }
    }
  }

  // TODO: Uncomment.
  //::glDeleteBuffers(n, bufIds);
}