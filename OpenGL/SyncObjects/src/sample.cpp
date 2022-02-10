
#include <GL/glew.h>
#include <SDL.h>
#include <filesystem>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#include <limits>
#include <memory>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

#define NOMINMAX
#include "portable-file-dialogs.h"

#include "json.hpp"

#include <sampleLibFile.h>

// Begin sample specific code
// Globals for SyncThread
/* Fencing state. */
static bool useFence = true;
unsigned char bakedColours[] =
{
    255,    0,      0,    255,
      0,  255,    0,    255,
    255,  255,    0,    255,
      0,  255,    255,    255,
};

unsigned char *textureData = nullptr;

static EGLDisplay mainDisplay:
static EGLContext mainContext:
static EGLSurface pBufferSurface = nullptr;
static EGLContext pBufferContext = nullptr;
bool exitThread = false;

/* Sync objects. */
GLsync secondThreadSyncObj = NULL;
GLsync mainThreadSyncObj = NULL;
GLuint64 timeout = GL_TIMEOUT_IGNORED;

int swapStripes = -1;
int texWidth = 512;
int texHeight = 512;

#define GL_CHECK(x) \
    { \
        GLenum glError = glGetError(); \
        if(glError != GL_NO_ERROR) { \
            LOG("glGetError() = %i (0x%.8x) at %s:%i\n", glError, glError, __FILE__, __LINE__); \
            exit(1); \
        } \
    }

GLuint iCubeTex = 0;

void LOG(const char* format, ...)
{
    char dest[1024 * 16];
    va_list argptr;
    va_start(argptr, format);
    vsprintf(dest, format, argptr);
    va_end(argptr);
    printf(dest);
}

/* Initialise texture buffer data. */
void AnimateTexture(void);

void InitTexture(void)
{
    int bytesPerPixel = 4;
    int numBytes = texWidth * texHeight * bytesPerPixel;

    /* Allocate data buffer. */
    textureData = (unsigned char*) malloc(numBytes * sizeof(unsigned char));

    if(!textureData)
    {
        LOG("Could not allocate memory for texture data.");
        exit(1);
    }

    /* Fill texture buffer with data. Circles with different colours. */
    AnimateTexture();

    /* Initialise texture. */
    GL_CHECK(glGenTextures(1, &iCubeTex));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, iCubeTex));

    /* Upload texture. */
    GL_CHECK(glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData));

    /* Set filtering. */
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
}

/* Modify the texture. */
void AnimateTexture()
{
    static int col = 0;
    static int col1 = 1;
    static int col2= 2;
    static int col3= 3;

    float r1 = texHeight / 16;
    float r2 = texHeight / 8;
    float r3 = texHeight / 4;
    float r4 = texHeight / 2;
    float r5 = texHeight / 1;

    float r12 = r1 * r1;
    float r22 = r2 * r2;
    float r32 = r3 * r3;
    float r42 = r4 * r4;
    float r52 = r5 * r5;

    int offset = 0;
    float d2 = 0.0f;

    for(int y = 0; y < texHeight; y++)
    {
        for(int x = 0; x < texWidth; x++)
        {
            /* Squared distance from pixel to centre. */
            d2 = (y - texHeight / 2) * (y - texHeight / 2) + (x - texWidth / 2) * (x - texWidth / 2);

            col = col % 4;
            col1 = col1 % 4;
            col2 = col2 % 4;
            col3 = col3 % 4;

            if(d2 < r12)
            {
                textureData[offset] =     bakedColours[4 * col+0];
                textureData[offset + 1] = bakedColours[4 * col+1];
                textureData[offset + 2] = bakedColours[4 * col+2];
                textureData[offset + 3] = bakedColours[4 * col + 3];
            }
            else if(d2 < r22)
            {

                textureData[offset] =     bakedColours[4 * (col1) + 0];
                textureData[offset + 1] = bakedColours[4 * col1 + 1];
                textureData[offset + 2] = bakedColours[4 * col1 + 2];
                textureData[offset + 3] = bakedColours[4 * col1 + 3];
            }
            else if(d2 < r32)
            {
                textureData[offset] =     bakedColours[4 * col2 + 0];
                textureData[offset + 1] = bakedColours[4 * col2 + 1];
                textureData[offset + 2] = bakedColours[4 * col2 + 2];
                textureData[offset + 3] = bakedColours[4 * col2 + 3];
            }
            else if(d2 < r42)
            {
                textureData[offset] =     bakedColours[4 * col3 + 0];
                textureData[offset + 1] = bakedColours[4 * col3 + 1];
                textureData[offset + 2] = bakedColours[4 * col3 + 2];
                textureData[offset + 3] = bakedColours[4 * col3 + 3];
            }
            else if(d2 < r52)
            {
                textureData[offset] =     128;
                textureData[offset + 1] = 128;
                textureData[offset + 2] = 128;
                textureData[offset + 3] = 255;
            }

            offset += 4;
        }
    }

    col++;
    col1++;
    col2++;
    col3++;
}

/* Secondary thread's working function. */
static void *textureAnimationThread(void *arg)
{
    /* Initialise main display and context variables. */
    mainDisplay = eglGetCurrentDisplay();
    mainContext = eglGetCurrentContext();

    /* Secondary thread's surface and rendering context creation. */
    EGLConfig config = findConfig(mainDisplay, true, true);
    pBufferSurface = eglCreatePbufferSurface(mainDisplay, config, pBufferAttributes);
    if(pBufferSurface == EGL_NO_SURFACE)
    {
        GLint error = glGetError();
        LOG("glGetError(): %i (0x%.4x)\n", (int)error, (int)error);
        LOG("Failed to create EGL pixel buffer surface at %s:%i\n", __FILE__, __LINE__);
        exit(1);
    }

    LOG("PBuffer surface created successfully.\n");

    /* Unconditionally bind to OpenGL ES API. */
    eglBindAPI(EGL_OPENGL_ES_API);

    /* [Creating rendering context] */
    /* Sharing OpenGL ES objects with main thread's rendering context. */
    pBufferContext = eglCreateContext(mainDisplay, config, mainContext, contextAttributes);
    /* [Creating rendering context] */
    if(pBufferContext == EGL_NO_CONTEXT)
    {
        GLint error = glGetError();
        LOG("glGetError(): %i (0x%.4x)\n", (int)error, (int)error);
        LOG("Failed to create EGL pBufferContext at %s:%i\n", __FILE__, __LINE__);
        exit(1);
    }

    LOG("PBuffer context created successfully sharing GLES objects with the main context.\n");
    GL_CHECK(eglMakeCurrent(mainDisplay, pBufferSurface, pBufferSurface, pBufferContext));
    LOG("PBuffer context made current successfully.\n");

    /*
     * Flags to pass to glFenceSync must be zero as there are no flag defined yet.
     * The condition must be set to GL_SYNC_GPU_COMMANDS_COMPLETE.
     */
    GLbitfield flags = 0;
    GLenum condition = GL_SYNC_GPU_COMMANDS_COMPLETE;

    while(!exitThread)
    {
        /* Set texture change frequency to 60 frames/s. */
        usleep(1000000 / 60);

        /* Change texture. */
        AnimateTexture();

        if(useFence)
        {
            if (mainThreadSyncObj != NULL)
            {
                GL_CHECK(glWaitSync(mainThreadSyncObj, flags, timeout));
            }
            else
            {
                continue;
            }

            /* Upload texture. */
            GL_CHECK(glBindTexture(GL_TEXTURE_2D, iCubeTex));
            GL_CHECK(glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData));

            /* This fence create a sync object which is signalled when the fence command reaches the end of the graphic pipeline. */
            secondThreadSyncObj = glFenceSync(condition, flags);

            GLint error = glGetError();
            if (secondThreadSyncObj == NULL || error == GL_INVALID_ENUM  || error == GL_INVALID_VALUE )
            {
                LOG( "glFenceSync failed at textureAnimationThread.\n");
            }
        }
        else
        {
            /* Upload texture. */
            GL_CHECK(glBindTexture(GL_TEXTURE_2D, iCubeTex));
            GL_CHECK(glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData));
        }
    }

    LOG("Exiting secondary thread.\n");

    return NULL;
}
// End sample specific code

void Init(RenderContext* renderContext) {

    // Set the default draw parameters.
    glFrontFace(GL_CW);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_FRAMEBUFFER_SRGB);

    // Load the shaders.
    LoadShader("../Shaders/threadsync.vs", GL_VERTEX_SHADER, renderContext->m_shaderMesh.m_vertexShader);
    LoadShader("../Shaders/threadsync.fs", GL_FRAGMENT_SHADER, renderContext->m_shaderMesh.m_fragmentShader);

    // Link the shaders.
    LinkShaders({renderContext->m_shaderMesh.m_vertexShader, renderContext->m_shaderMesh.m_fragmentShader},
                renderContext->m_shaderMesh.m_program);

    // Get the uniform locations from the mesh shader.
    renderContext->m_shaderMesh.m_locProjViewWorld =
        glGetUniformLocation(renderContext->m_shaderMesh.m_program, "projViewWorld");
    renderContext->m_shaderMesh.m_locViewWorld =
        glGetUniformLocation(renderContext->m_shaderMesh.m_program, "viewWorld");
    renderContext->m_shaderMesh.m_locBaseColor =
        glGetUniformLocation(renderContext->m_shaderMesh.m_program, "baseColor");
    renderContext->m_shaderMesh.m_locDiffuseTexture =
        glGetUniformLocation(renderContext->m_shaderMesh.m_program, "diffuseTexture");
    renderContext->m_shaderMesh.m_locLightDir = glGetUniformLocation(renderContext->m_shaderMesh.m_program, "lightDir");

    // Get the uniform locations from the texture sampling shader.
    renderContext->m_shaderTextureSampling.m_locDiffuseTexture =
        glGetUniformLocation(renderContext->m_shaderTextureSampling.m_program, "diffuseTexture");

    // Create the meshes.
    CreateMeshQuad(renderContext, renderContext->m_quad);
    CreateMeshOBJ("../Assets/Models/Sponza/sponza.obj", renderContext->m_mesh);

    // Create the white texture.
    const uint8_t whiteTextureData[4] = {255, 255, 255, 255};
    glGenTextures(1, &renderContext->m_whiteTexture);
    glBindTexture(GL_TEXTURE_2D, renderContext->m_whiteTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whiteTextureData);

    // Create the color render targets.
    CreateRenderTarget(renderContext->m_screenWidth / 2, renderContext->m_screenHeight / 2, GL_RGBA8, true,
                       renderContext->m_colorRenderTarget);

    // Set the default camera transformation.
    renderContext->m_camera.Move({0, 0, 2});
}

void Shutdown(RenderContext* renderContext) {

    glDeleteShader(renderContext->m_shaderMesh.m_vertexShader);
    glDeleteShader(renderContext->m_shaderMesh.m_fragmentShader);
    glDeleteProgram(renderContext->m_shaderMesh.m_program);
    glDeleteTextures(1, &renderContext->m_whiteTexture);
    DeleteMesh(renderContext->m_mesh);
    DeleteAppMode(renderContext);
    glDeleteShader(renderContext->m_fullscreenQuadVertexShader);
    glDeleteShader(renderContext->m_shaderTextureSampling.m_fragmentShader);
    glDeleteProgram(renderContext->m_shaderTextureSampling.m_program);
    DeleteMesh(renderContext->m_quad);
    glDeleteTextures(1, &renderContext->m_currentFrameColorTexture);

    glDeleteFramebuffers(1, &renderContext->m_colorRenderTarget.m_fbo);
    glDeleteTextures(1, &renderContext->m_colorRenderTarget.m_texture);
}

void Render(RenderContext* renderContext) {

    // Enable the MSAA anti-aliasing.
    glEnable(GL_MULTISAMPLE);

    // Bind the default frame buffer.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Set the viewport.
    glViewport(0, 0, renderContext->m_screenWidth, renderContext->m_screenHeight);

    // Clear the viewport.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the shader program.
    glUseProgram(renderContext->m_shaderMesh.m_program);

    // Set the matrix shader parameters.
    const float rotationAngle = 0.0f;  // TODO: Load from a config file?
    const glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    const glm::mat4 rotation = glm::mat4(glm::quat(glm::vec3(0.0f, rotationAngle, 0.0f)));
    const glm::mat4 world = glm::scale(translation * rotation, glm::vec3(0.05f));
    const glm::mat4 view = renderContext->m_camera.ComputeViewMatrix();
    const glm::mat4 proj = renderContext->m_camera.ComputeProjectionMatrix(renderContext->m_screenWidth,
                                                                            renderContext->m_screenHeight);
    const glm::mat4 viewWorld = view * world;
    const glm::mat4 projViewWorld = proj * viewWorld;
    glUniformMatrix4fv(renderContext->m_shaderMesh.m_locProjViewWorld, 1, GL_FALSE, glm::value_ptr(projViewWorld));
    glUniformMatrix4fv(renderContext->m_shaderMesh.m_locViewWorld, 1, GL_FALSE, glm::value_ptr(viewWorld));

    // Set the light direction shader parameter.
    const glm::quat lightRotation(glm::vec3(-glm::radians(45.0f), -glm::radians(45.0f), 0.0f));
    const glm::vec3 lightDirectionWorldSpace = lightRotation * glm::vec3(0.0f, 0.0f, 1.0f);
    const glm::vec3 lightDirectionViewSpace = view * glm::vec4(lightDirectionWorldSpace, 0.0f);
    glUniform3fv(renderContext->m_shaderMesh.m_locLightDir, 1, &lightDirectionViewSpace[0]);

    // Render the mesh.
    RenderMesh(renderContext, renderContext->m_mesh);
}

void RenderGUI(RenderContext* renderContext, uint32_t fps) {
    // Disable the SRGB to have proper IMGUI theme colors.
    glDisable(GL_FRAMEBUFFER_SRGB);

    // Init the IMGUI frame.
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("info", NULL,
                 ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoMove);
    ImGui::SetWindowPos(ImVec2(0, 0));
    ImGui::SetWindowSize(ImVec2(renderContext->m_screenWidth, 200));
    ImGui::Text("FPS: %d", fps);
    std::string modesStr;
    for (int32_t i = 0; i < AppMode::NumModes; ++i) {
        char s[100];
        std::sprintf(s, "[%d] %s   ", i + 1, GetAppModeStr(static_cast<AppMode>(i)).c_str());
        modesStr += s;
    }
    ImGui::Text("%s", modesStr.c_str());
    const std::string currentModeStr = GetAppModeStr(renderContext->m_appMode);
    ImGui::Text("Current mode: %s", currentModeStr.c_str());

    renderContext->m_appModeImpl->RenderGUI();

    ImGui::End();

    // Status bar.
    ImGui::Begin("statusbar", NULL,
                 ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoMove);
    ImGui::SetWindowPos(ImVec2(0, renderContext->m_screenHeight - 64));
    ImGui::SetWindowSize(ImVec2(renderContext->m_screenWidth / 2, 64));
    renderContext->m_appModeImpl->RenderStatusBarGUI();
    ImGui::End();

    // Render.
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Enable the SRGB to have proper scene colors.
    glEnable(GL_FRAMEBUFFER_SRGB);
}

void UpdateEventsForAppMode(RenderContext* renderContext, SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        const int32_t i = event.key.keysym.sym - SDLK_1;
        if (i >= 0 && i < AppMode::NumModes) {
            SetAppMode(renderContext, static_cast<AppMode>(i));
        }
    }
}

int main(int argc, char* argv[]) {
    // Init SDL.
    SDL_Init(SDL_INIT_VIDEO);

    // Set the OpenGL attributes.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetSwapInterval(0);  // 1 to enable VSync.

    // Create the window and the OpenGL context.
    constexpr uint32_t screenWidth = 1280;
    constexpr uint32_t screenHeight = 720;
    SDL_Window* window = SDL_CreateWindow("Samples", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          screenWidth, screenHeight, SDL_WINDOW_OPENGL);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    // Init Glew.
    glewInit();

    // Flip vertically when load an image.
    stbi_set_flip_vertically_on_load(1);

    // Flip vertically when write to file.
    stbi_flip_vertically_on_write(1);

    // Create and init the render context.
    RenderContext renderContext;
    renderContext.m_screenWidth = screenWidth;
    renderContext.m_screenHeight = screenHeight;
    Init(&renderContext);

    // Begin sample specific code
    InitTexture();

    // ThreadSync: Create the texture animation thread.
    std::thread textureAnimation(textureAnimationThread, nullptr);
    // End sample specific code

    // Init IMGUI.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Variables used to compute the FPS.
    uint32_t fps = 0;
    uint32_t frameCounter = 0;
    uint32_t fpsTimerStartTime = SDL_GetTicks();

    // Variables used to compute the delta time.
    uint32_t lastTime = SDL_GetTicks();
    float dt = 0.0f;

    // Set the default app mode.
    SetAppMode(&renderContext, AppMode::FreeCamera);

    // Main loop.
    bool isRunning = true;
    while (isRunning) {
        // Compute the delta time.
        uint32_t now = SDL_GetTicks();
        uint32_t diff = now - lastTime;
        lastTime = now;
        dt = static_cast<float>(diff) * 0.001f;

        // Update the FPS.
        if (now - fpsTimerStartTime >= 1000) {
            fpsTimerStartTime = SDL_GetTicks();
            fps = frameCounter;
            frameCounter = 0;
        }
        // Process the events.
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (!renderContext.m_appModeImpl->UpdateEvents(event)) {
                UpdateEventsForAppMode(&renderContext, event);
            }
            switch (event.type) {
                case SDL_KEYDOWN: {
                    if (event.key.state == SDL_PRESSED) {
                        const SDL_Keycode keyCode = event.key.keysym.sym;
                        if (keyCode == SDLK_ESCAPE) {
                            isRunning = false;
                        }
                    }
                    break;
                }
                case SDL_WINDOWEVENT: {
                    if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                        isRunning = false;
                    }
                    break;
                }
                default: {
                    break;
                }
            }
        }

        // Update the app modes.
        renderContext.m_appModeImpl->Update(dt);

        // Render the frame.
        Render(&renderContext);
        RenderGUI(&renderContext, fps);

        // Swap the OpenGL buffers.
        SDL_GL_SwapWindow(window);

        // Add a frame in the counter.
        ++frameCounter;
    }

    // Shutdown.
    Shutdown(&renderContext);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    // Done.
    return EXIT_SUCCESS;
}
