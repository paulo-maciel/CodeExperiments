// dear imgui: Renderer Backend for modern OpenGL with shaders / programmatic pipeline
// - Desktop GL: 2.x 3.x 4.x
// - Embedded GL: ES 2.0 (WebGL 1.0), ES 3.0 (WebGL 2.0)
// This needs to be used along with a Platform Backend (e.g. GLFW, SDL, Win32, custom..)

// Implemented features:
//  [X] Renderer: User texture binding. Use 'GLuint' OpenGL texture identifier as void*/ImTextureID. Read the FAQ about ImTextureID!
//  [x] Renderer: Desktop GL only: Support for large meshes (64k+ vertices) with 16-bit indices.

// You can use unmodified imgui_impl_* files in your project. See examples/ folder for examples of using this.
// Prefer including the entire imgui/ repository into your project (either as a copy or as a submodule), and only build the backends you need.
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// About GLSL version:
//  The 'glsl_version' initialization parameter should be NULL (default) or a "#version XXX" string.
//  On computer platform the GLSL version default to "#version 130". On OpenGL ES 3 platform it defaults to "#version 300 es"
//  Only override if your GL version doesn't handle this GLSL version. See GLSL version table at the top of imgui_impl_opengl3.cpp.

#pragma once
#include "imgui.h"      // IMGUI_IMPL_API

// Backend API
IMGUI_IMPL_API bool     ImGui_ImplMLGL_Init(const char* glsl_version = NULL);
IMGUI_IMPL_API void     ImGui_ImplMLGL_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplMLGL_NewFrame(const float& viewportWidth, const float& viewportHeight);
IMGUI_IMPL_API void     ImGui_ImplMLGL_NewFrame(const float& viewportWidth, const float& viewportHeight, const float* customModelViewProjection);
IMGUI_IMPL_API void     ImGui_ImplMLGL_RenderDrawData(ImDrawData* draw_data);

IMGUI_IMPL_API int32_t  ImGui_ImplMLGL_HandleInputEvent(const float& pointerPosX, const float& pointerPosY, const bool& triggerIsDown); // viewport coords

// (Optional) Called by Init/NewFrame/Shutdown
IMGUI_IMPL_API bool     ImGui_ImplMLGL_CreateFontsTexture();
IMGUI_IMPL_API void     ImGui_ImplMLGL_DestroyFontsTexture();
IMGUI_IMPL_API bool     ImGui_ImplMLGL_CreateDeviceObjects();
IMGUI_IMPL_API void     ImGui_ImplMLGL_DestroyDeviceObjects();
