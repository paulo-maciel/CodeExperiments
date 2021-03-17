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
  GLenum type_;
  int id_;
  size_t size_;
};

class DebugGpuAlloc {
  public:
    static DebugGpuAlloc& Get();

    void glGenTextures(	GLsizei n, GLuint* textures);

    void glTextureStorage2D(GLuint texture, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);

    void glBindTexture(	GLenum target, GLuint texture);
    void glTexStorage2D(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);

    void glDeleteTextures( GLsizei n, const GLuint * textures);

  public:
    // TODO: Make private.
    size_t getAllocated(pid_t pid) const;
    size_t getSize(pid_t pid, GLenum target, GLuint texture) const;

  private:
    DebugGpuAlloc();
    ~DebugGpuAlloc();

    size_t computeSize(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);
    uint32_t getHash(GLenum type_, int id_) const;

    // Bound texture.
    GLuint boundTexture_;
    GLenum boundTarget_;

    std::unordered_map<pid_t, std::unordered_map<uint32_t, GpuData>> allocMap_;

    std::unordered_map<pid_t, std::deque<GLuint>> textureIDs_;
};

