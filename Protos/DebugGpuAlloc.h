// [%COPYRIGHT_BEGIN%] -------------------------------------------------
//
// Copyright (c) 201x Magic Leap, Inc. (COMPANY) All Rights Reserved.
// Magic Leap, Inc. Confidential and Proprietary
//
// NOTICE: All information contained herein is, and remains the property
// of COMPANY. The intellectual and technical concepts contained herein
// are proprietary to COMPANY and may be covered by U.S. and Foreign
// Patents, patents in process, and are protected by trade secret or
// copyright law. Dissemination of this information or reproduction of
// this material is strictly forbidden unless prior written permission is
// obtained from COMPANY. Access to the source code contained herein is
// hereby forbidden to anyone except current COMPANY employees, managers
// or contractors who have executed Confidentiality and Non-disclosure
// agreements explicitly covering such access.
//
// The copyright notice above does not evidence any actual or intended
// publication or disclosure of this source code, which includes
// information that is confidential and/or proprietary, and is a trade
// secret, of COMPANY. ANY REPRODUCTION, MODIFICATION, DISTRIBUTION,
// PUBLIC PERFORMANCE, OR PUBLIC DISPLAY OF OR THROUGH USE OF THIS
// SOURCE CODE WITHOUT THE EXPRESS WRITTEN CONSENT OF COMPANY IS
// STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE LAWS AND
// INTERNATIONAL TREATIES. THE RECEIPT OR POSSESSION OF THIS SOURCE
// CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS
// TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE,
// USE, OR SELL ANYTHING THAT IT MAY DESCRIBE, IN WHOLE OR IN PART.
// ---------------------------------------------------------------------
//
// %RUNTIME_TITLE%
// %RUNTIME_DATE%
// %RUNTIME_VERSION%
//
// [%COPYRIGHT_END%] ---------------------------------------------------
#pragma once

#include <GL/gl.h>


#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <unordered_map>
#include <string>
#include <deque>


struct GpuData {
  GLenum target_;
  int id_;
  size_t size_;
};

class DebugGpuAlloc {
  public:
    static const std::unordered_map<GLenum, std::string> s_targets;
    static const std::array<GLenum, 10> s_texTargets;
    static const std::array<GLenum, 5> s_bufTargets;

    static DebugGpuAlloc& Get();

    // Textures API
    void glGenTextures(	GLsizei n, GLuint* texIds);
    void glBindTexture(	GLenum target, GLuint id);
    void glTexImage2D(	GLenum target, GLint level, GLint internalformat,
                        GLsizei width, GLsizei height,
                        GLint border, GLenum format,
                        GLenum type, const void * data);
    void glTextureStorage2D(GLuint id, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);
    void glTexStorage2D(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);
    void glDeleteTextures( GLsizei n, const GLuint * textures);

    // Buffers API
    void glGenBuffers(GLsizei n, GLuint* bufferIds);
    void glBindBuffer(GLenum target, GLuint id);
    void glBufferData(GLenum target, GLsizei size, const void* data, GLenum usage);
    void glBufferStorage(	GLenum target, GLsizeiptr size, const void * data, GLbitfield flags);
    void glDeleteBuffers(GLsizei n, GLuint* bufIds);

  public:
    // TODO: Make private.
    size_t getAllocated(pid_t pid) const;
    size_t getSize(pid_t pid, GLenum target, GLuint texture) const;
    bool addBuffer(GLenum target, GLsizei size);

  private:
    DebugGpuAlloc();
    ~DebugGpuAlloc();

    size_t computeTexSize(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);
    uint32_t getHash(GLenum target_, int id_) const;

    // Bound texture.
    GLuint boundTexture_;
    GLenum boundTexTarget_;

    // Bound buffer.
    GLuint boundBuffer_;
    GLenum boundBufTarget_;

    std::unordered_map<pid_t, std::unordered_map<uint32_t, GpuData>> allocMap_;
};

