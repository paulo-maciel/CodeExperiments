﻿
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


struct Material {
    glm::vec3 m_baseColor = glm::vec3(1.0f);
    GLuint m_diffuseTexture = 0;
};

struct SubMesh {
    int32_t m_materialIndex = -1;
    uint32_t m_indexStart = 0;
    uint32_t m_indexCount = 0;
};

struct Mesh {
    std::vector<Material> m_materials;
    std::vector<SubMesh> m_subMeshes;
    GLuint m_vertexArray = 0;
    GLuint m_vertexBuffer = 0;
    GLuint m_indexBuffer = 0;
};

struct ShaderMesh {
    GLuint m_vertexShader = 0;
    GLuint m_fragmentShader = 0;
    GLuint m_program = 0;
    GLint m_locProjViewWorld = -1;
    GLint m_locViewWorld = -1;
    GLint m_locBaseColor = -1;
    GLint m_locDiffuseTexture = -1;
    GLint m_locLightDir = -1;
};

struct ShaderTextureSampling {
    GLuint m_fragmentShader = 0;
    GLuint m_program = 0;
    GLint m_locDiffuseTexture = -1;
};

struct RenderTarget {
    GLuint m_fbo = 0;
    GLuint m_texture = 0;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
};

struct Camera {
    glm::vec3 m_position = glm::vec3(0.0f);
    glm::quat m_rotation = glm::quat(glm::vec3(0.0f));

    glm::mat4 ComputeViewMatrix() const {
        const glm::mat4 world = glm::translate(glm::mat4(1.0f), m_position) * glm::mat4(m_rotation);
        return glm::inverse(world);
    }

    glm::mat4 ComputeProjectionMatrix(uint32_t screenWidth, uint32_t screenHeight) const {
        return glm::perspective(glm::radians(45.0f), static_cast<float>(screenWidth) / static_cast<float>(screenHeight),
                                0.1f, 1000.0f);
    }

    void Move(const glm::vec3& dir) { m_position += m_rotation * dir; }

    void Reset() {
        m_position = glm::vec3(0.0f);
        m_rotation = glm::quat(glm::vec3(0.0f));
    }
};

enum AppMode { FreeCamera, RecordCameraTrack, PlaybackCameraTrack, RecordFrames, ReprojectionViewer, NumModes };

std::string GetAppModeStr(AppMode am) {
    switch (am) {
        case AppMode::FreeCamera:
            return "Free camera movement";
        case AppMode::RecordCameraTrack:
            return "Record camera track";
        case AppMode::PlaybackCameraTrack:
            return "Playback camera track";
        case AppMode::RecordFrames:
            return "Record frames";
        case AppMode::ReprojectionViewer:
            return "Reprojection Viewer";
        default:
            return "Invalid";
    }
}

class AppModeImplementation;

struct RenderContext {
    uint32_t m_screenWidth = 0;
    uint32_t m_screenHeight = 0;
    bool m_mustReloadReprojection = false;
    GLuint m_whiteTexture = 0;
    Mesh m_quad;
    Mesh m_mesh;
    Camera m_camera;
    AppMode m_appMode = AppMode::FreeCamera;
    std::unique_ptr<AppModeImplementation> m_appModeImpl;
    std::unique_ptr<ml::Reprojection::Performance::GLDebugOutput> m_glDebugOutput;
    GLuint m_fullscreenQuadVertexShader = 0;
    ShaderTextureSampling m_shaderTextureSampling;
    ShaderMesh m_shaderMesh;
    GLuint m_currentFrameColorTexture = 0;
    GLuint m_currentFrameDepthTexture = 0;
    float m_positionOffset[3] = {};
    float m_rotationOffset[3] = {};
    int32_t m_frameDelay = 0;
    int32_t m_algorithmIndex = 2;
    int32_t m_currentAlgorithmIndex = m_algorithmIndex;
    float m_disparityThreshold = 0.1f;
    int32_t m_depthDispartiyStepPOT = 1;  // Power-of-two step
    ml::Reprojection::DepthDisparityAlgorithm m_depthDisparityAlgorithm =
        ml::Reprojection::DepthDisparityAlgorithm::DepthSectionProjection;
    int32_t m_colorDownSampleIndex = 0;
    int32_t m_depthDownSampleIndex = 0;
    int32_t m_currentDepthDownSampleIndex = m_depthDownSampleIndex;
    RenderTarget m_colorDownSamplingRenderTarget;
    RenderTarget m_colorUpSamplingRenderTarget;
    RenderTarget m_depthDownSamplingRenderTargets[6];
    RenderTarget m_depthUpSamplingRenderTarget;
    float m_depthCompressionMin = 2.5f;
    float m_depthCompressionMax = 5.0f;
    bool m_wireframe = false;
    bool m_cullBackground = true;
};

void DeleteMesh(Mesh& mesh);
void CreateMeshOBJ(const std::string& filename, Mesh& mesh);
bool LoadTexture(const char* filename, bool depthTexture, bool mipmaps, bool clamp, bool linear, GLuint& textureID);

class AppModeImplementation {
   public:
    AppModeImplementation(RenderContext* rc) : m_renderContext(rc) {}
    virtual bool UpdateEvents(const SDL_Event& event) { return false; }
    virtual void Update(float dt) {}
    virtual void RenderGUI() {}
    virtual void RenderStatusBarGUI() {}

   protected:
    RenderContext* m_renderContext = nullptr;
};

class FreeCameraMovement final : public AppModeImplementation {
   public:
    FreeCameraMovement(RenderContext* rc) : AppModeImplementation(rc) {
        m_cameraAngles = glm::degrees(glm::eulerAngles(m_renderContext->m_camera.m_rotation));
    }

    void RenderGUI() override {
        if (m_renderContext->m_appMode == AppMode::FreeCamera) {
            ImGui::Text(
                "[W,A,S,D,Q,E,MouseWheel] Move   [MousePress] Camera look around   [Square Brackets] Decrease, "
                "Increase "
                "Speed (%.2f)   [L] Load model",
                m_movingSpeed);
        } else {
            ImGui::Text(
                "[W,A,S,D,Q,E,MouseWheel] Move   [MousePress] Camera look around   [Square Brackets] Decrease, "
                "Increase "
                "Speed (%.2f)",
                m_movingSpeed);
        }
    }

    bool UpdateEvents(const SDL_Event& event) override {
        bool processed = false;

        ImGuiIO& io = ImGui::GetIO();

        switch (event.type) {
            case SDL_KEYUP:
            case SDL_KEYDOWN: {
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                        m_keyCommandState[MoveForward] = event.key.state == SDL_PRESSED;
                        processed = true;
                        break;
                    case SDLK_s:
                        m_keyCommandState[MoveBackward] = event.key.state == SDL_PRESSED;
                        processed = true;
                        break;
                    case SDLK_a:
                        m_keyCommandState[MoveLeft] = event.key.state == SDL_PRESSED;
                        processed = true;
                        break;
                    case SDLK_d:
                        m_keyCommandState[MoveRight] = event.key.state == SDL_PRESSED;
                        processed = true;
                        break;
                    case SDLK_e:
                        m_keyCommandState[MoveUp] = event.key.state == SDL_PRESSED;
                        processed = true;
                        break;
                    case SDLK_q:
                        m_keyCommandState[MoveDown] = event.key.state == SDL_PRESSED;
                        processed = true;
                        break;
                    case SDLK_LEFTBRACKET:
                        m_keyCommandState[DecreaseSpeed] = m_keyCommandState[DecreaseSpeed] == false &&
                                                           event.type == SDL_KEYDOWN && event.key.state == SDL_PRESSED;
                        processed = true;
                        break;
                    case SDLK_RIGHTBRACKET:
                        m_keyCommandState[IncreaseSpeed] = m_keyCommandState[IncreaseSpeed] == false &&
                                                           event.type == SDL_KEYDOWN && event.key.state == SDL_PRESSED;
                        processed = true;
                        break;
                    case SDLK_LSHIFT:
                        m_keyCommandState[Turbo] = event.key.state == SDL_PRESSED;
                        processed = true;
                        break;
                    case SDLK_LCTRL:
                        m_keyCommandState[Crawl] = event.key.state == SDL_PRESSED;
                        processed = true;
                        break;
                    case SDLK_l:
                        if (event.key.state == SDL_PRESSED) {
                            if (m_renderContext->m_appMode == AppMode::FreeCamera) {
                                auto fd = pfd::open_file("Load Model", ".", {"OBJ Files (.obj)", "*.obj"});
                                if (fd.result().empty() == false) {
                                    DeleteMesh(m_renderContext->m_mesh);
                                    CreateMeshOBJ(fd.result()[0].c_str(), m_renderContext->m_mesh);
                                    m_renderContext->m_camera.Reset();
                                    m_renderContext->m_camera.Move({0, 0, 2});
                                }
                            }
                            processed = true;
                        }
                }
                break;
            }
            case SDL_MOUSEBUTTONDOWN: {
                if (m_keyCommandState[MouseButton] == false && !io.WantCaptureMouse) {
                    m_mouseDownPosition = glm::vec2(event.button.x, event.button.y);
                    m_mouseDownCameraAngles = m_cameraAngles;
                    m_keyCommandState[MouseButton] = true;
                    processed = true;
                }
                break;
            }
            case SDL_MOUSEBUTTONUP: {
                if(!io.WantCaptureMouse) {
                    m_keyCommandState[MouseButton] = false;
                    processed = true;
                }
                break;
            }
            case SDL_MOUSEMOTION: {
                if (m_keyCommandState[MouseButton] && !io.WantCaptureMouse) {
                    const glm::vec2 mousePos = glm::vec2(event.motion.x, event.motion.y);
                    glm::vec2 diff = mousePos - m_mouseDownPosition;
                    diff.x /= static_cast<float>(m_renderContext->m_screenWidth);
                    diff.y /= static_cast<float>(m_renderContext->m_screenHeight);

                    const float angleX = -diff.y * 90.0f;
                    const float angleY = -diff.x * 90.0f;
                    m_cameraAngles = m_mouseDownCameraAngles + glm::vec3(angleX, angleY, 0.0f);
                    m_cameraAngles.x = glm::clamp(m_cameraAngles.x, -90.0f, 90.0f);
                    m_renderContext->m_camera.m_rotation = glm::quat(glm::radians(m_cameraAngles));
                    processed = true;
                }
                break;
            };
            case SDL_MOUSEWHEEL: {
                if(!io.WantCaptureMouse) {
                    m_keyCommandState[MoveForwardWheel] = event.wheel.y > 0.0f;
                    m_keyCommandState[MoveBackwardWheel] = event.wheel.y < 0.0f;
                    m_wheelSteps = abs(event.wheel.y) * 20.0f;
                }
                break;
            }
            default: {
                break;
            }
        }
        return processed;
    }

    void Update(float dt) override {
        glm::vec3 targetSpeed{};
        targetSpeed.z = (m_keyCommandState[MoveForward] ? -1 : 0) + (m_keyCommandState[MoveBackward] ? +1 : 0) +
                        (m_keyCommandState[MoveForwardWheel] ? -m_wheelSteps : 0) +
                        (m_keyCommandState[MoveBackwardWheel] ? +m_wheelSteps : 0);
        targetSpeed.x = (m_keyCommandState[MoveLeft] ? -1 : 0) + (m_keyCommandState[MoveRight] ? +1 : 0);
        targetSpeed.y = (m_keyCommandState[MoveUp] ? +1 : 0) + (m_keyCommandState[MoveDown] ? -1 : 0);

        constexpr auto speedAdjustment = 1.35f;
        m_movingSpeed *= (m_keyCommandState[IncreaseSpeed] ? speedAdjustment : 1) *
                         (m_keyCommandState[DecreaseSpeed] ? 1.f / speedAdjustment : 1);

        targetSpeed *= glm::vec3{1.f, 0.5f, 1.f} * m_movingSpeed;

        m_keyCommandState[MoveForwardWheel] = false;
        m_keyCommandState[MoveBackwardWheel] = false;
        m_keyCommandState[IncreaseSpeed] = false;
        m_keyCommandState[DecreaseSpeed] = false;

        // TODO: Maybe load from a config file if the movement is inertial or not.
        m_Speed = glm::mix(m_Speed, targetSpeed, 20.0f * dt);

        constexpr auto turboAccel = 5.f;
        constexpr auto crawlAccel = 0.25f;
        auto acceleration = (m_keyCommandState[Turbo] ? turboAccel : 1) * (m_keyCommandState[Crawl] ? crawlAccel : 1);

        m_renderContext->m_camera.Move(m_Speed * acceleration * dt);
    }

    void FromViewMatrix(glm::mat4 viewMat) {
        glm::mat4 camMat = glm::inverse(viewMat);
        m_renderContext->m_camera.m_position = camMat[3];
        m_renderContext->m_camera.m_rotation = glm::quat_cast(camMat);
        m_cameraAngles = glm::degrees(glm::eulerAngles(m_renderContext->m_camera.m_rotation));
    }

   private:
    enum KeyCommands {
        MoveForward,
        MoveBackward,
        MoveForwardWheel,
        MoveBackwardWheel,
        MoveLeft,
        MoveRight,
        MoveUp,
        MoveDown,
        MouseButton,
        IncreaseSpeed,
        DecreaseSpeed,
        Turbo,
        Crawl,
        NumKeyCommands
    };
    float m_movingSpeed = 5.0f;  // TODO: Load from a config file eventually.
    bool m_keyCommandState[NumKeyCommands] = {false};
    float m_wheelSteps = 1.0;
    glm::vec3 m_Speed = glm::vec3(0.0f);
    glm::vec2 m_mouseDownPosition = glm::vec2(0.0f);
    glm::vec3 m_cameraAngles = glm::vec3(0.0f);
    glm::vec3 m_mouseDownCameraAngles = glm::vec3(0.0f);
};

constexpr const char* g_jsonRootName = "CameraKeyframes";
std::string KeyframeRootNameToString(size_t index) { return "Key" + std::to_string(index); }

class AppModeImplementationImpl final : public AppModeImplementation {
   public:
    AppModeImplementationImpl(RenderContext* rc) : AppModeImplementation(rc) {
        m_freeCam = std::make_unique<FreeCameraMovement>(rc);
        LoadScenario("Capture/Scenario1");
    }

    void LoadScenario(const std::string& path) {
        // Load the current frame camera information.
        std::ifstream jsonFile(path + '/' + "Camera0.json");
        const nlohmann::json json = nlohmann::json::parse(jsonFile);
        jsonFile.close();
        glm::mat4 readViewMat;
        const std::string viewString = json["Camera"]["View"];
        std::sscanf(viewString.c_str(),
                    "mat4x4((%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f))",
                    &readViewMat[0][0], &readViewMat[0][1], &readViewMat[0][2], &readViewMat[0][3], &readViewMat[1][0],
                    &readViewMat[1][1], &readViewMat[1][2], &readViewMat[1][3], &readViewMat[2][0], &readViewMat[2][1],
                    &readViewMat[2][2], &readViewMat[2][3], &readViewMat[3][0], &readViewMat[3][1], &readViewMat[3][2],
                    &readViewMat[3][3]);

        glm::mat4 readProjMat;
        const std::string projectionString = json["Camera"]["Projection"];
        std::sscanf(projectionString.c_str(),
                    "mat4x4((%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f))",
                    &readProjMat[0][0], &readProjMat[0][1], &readProjMat[0][2], &readProjMat[0][3], &readProjMat[1][0],
                    &readProjMat[1][1], &readProjMat[1][2], &readProjMat[1][3], &readProjMat[2][0], &readProjMat[2][1],
                    &readProjMat[2][2], &readProjMat[2][3], &readProjMat[3][0], &readProjMat[3][1], &readProjMat[3][2],
                    &readProjMat[3][3]);

        m_currentProjectionMatrix = readProjMat;
        m_currentViewMatrix = readViewMat;

        m_freeCam->FromViewMatrix(readViewMat);

        // Load the textures.
        glDeleteTextures(1, &m_renderContext->m_currentFrameColorTexture);
        glDeleteTextures(1, &m_renderContext->m_currentFrameDepthTexture);
        LoadTexture((path + '/' + "ColorDump0.bmp").c_str(), false, false, true, true,
                    m_renderContext->m_currentFrameColorTexture);
        LoadTexture((path + '/' + "DepthDump0.bmp").c_str(), true, false, true, false,
                    m_renderContext->m_currentFrameDepthTexture);
    }

    void RenderGUI() override {
        m_freeCam->RenderGUI();
        ImGui::Text("[L] Load scenario");

        ml::Reprojection::RenderUI();

        // Reprojection Viewer UI.
        constexpr uint32_t windowHeight = 390;
        glm::vec2 windowPos = {10.0f, static_cast<float>(m_renderContext->m_screenHeight - windowHeight - 45)};
        glm::vec2 windowSize = {490.0f, static_cast<float>(windowHeight)};
        ImGui::SetNextWindowPos({windowPos.x, windowPos.y}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({windowSize.x, windowSize.y}, ImGuiCond_FirstUseEver);
        ImGui::Begin("Setup");
        ImGui::Text("SOURCE SETTINGS");
        constexpr const char* colorDownSampleListboxItems[] = {"Disabled", "Bilinear"};
        ImGui::Combo("Color Down-Sample", &m_renderContext->m_colorDownSampleIndex, colorDownSampleListboxItems, 2);
        constexpr const char* depthDownSampleListboxItems[] = {"Disabled",
                                                               "1/2",
                                                               "1/2 With Minimum Filter",
                                                               "1/4",
                                                               "1/4 With Minimum Filter",
                                                               "1/8",
                                                               "1/8 With Minimum Filter"};
        ImGui::Combo("Depth Down-Sample", &m_renderContext->m_depthDownSampleIndex, depthDownSampleListboxItems, 7);
        ImGui::SliderFloat("Depth Compression Min", &m_renderContext->m_depthCompressionMin, 0.1f, 50.0f);
        ImGui::SliderFloat("Depth Compression Max", &m_renderContext->m_depthCompressionMax, 0.1f, 50.0f);
        ImGui::Separator();
        ImGui::Text("ALGORITHM SETTINGS");
        constexpr const char* algorithmListboxItems[] = {"Disabled", "Pixel Forward", "Mesh Forward"};
        ImGui::Combo("Algorithm", &m_renderContext->m_algorithmIndex, algorithmListboxItems, 3);
        if (m_renderContext->m_algorithmIndex == 2) {
            ImGui::SliderFloat("Disparity Threshold", &m_renderContext->m_disparityThreshold, 0.001, 5.0, "%.4f",
                               ImGuiSliderFlags_Logarithmic);

            constexpr const char* ddSampleStepItems[] = {"1", "2", "4", "8"};
            if (ImGui::Combo("Depth Disparity Step", &m_renderContext->m_depthDispartiyStepPOT, ddSampleStepItems,
                             std::size(ddSampleStepItems))) {
                m_renderContext->m_mustReloadReprojection = true;
            }

            auto ddAlgorithmIdx = static_cast<int>(m_renderContext->m_depthDisparityAlgorithm);
            constexpr const char* ddAlgorithmListboxItems[] = {"Depth Section Projection",
                                                               "Neighbor Screen-Space Difference"};
            if (ImGui::Combo("Disparities Algorithm", &ddAlgorithmIdx, ddAlgorithmListboxItems,
                             std::size(ddAlgorithmListboxItems))) {
                m_renderContext->m_depthDisparityAlgorithm =
                    static_cast<ml::Reprojection::DepthDisparityAlgorithm>(ddAlgorithmIdx);
                m_renderContext->m_mustReloadReprojection = true;
            }
        }
        ImGui::Separator();
        ImGui::Text("TARGET SETTINGS");
        if (ImGui::Checkbox("Wireframe", &m_renderContext->m_wireframe)) {
            m_renderContext->m_mustReloadReprojection = true;
        }
        if (ImGui::Checkbox("Cull background", &m_renderContext->m_cullBackground)) {
            m_renderContext->m_mustReloadReprojection = true;
        }
        ImGui::Separator();
        ImGui::Text("LAG SETTINGS");
        ImGui::SliderFloat3("Position Offset", m_renderContext->m_positionOffset, -10.0f, 10.0f);
        ImGui::SliderFloat3("Rotation Offset", m_renderContext->m_rotationOffset, -180.0, 180.0f);
        ImGui::SliderInt("Frame Delay", &m_renderContext->m_frameDelay, 0, 10);
        ImGui::End();
    }

    bool UpdateEvents(const SDL_Event& event) override {
        switch (event.type) {
            case SDL_KEYDOWN: {
                if (event.key.state == SDL_PRESSED) {
                    const SDL_Keycode keyCode = event.key.keysym.sym;
                    if (keyCode == SDLK_l) {
                        auto fd = pfd::select_folder("Select Scenario Folder", ".");
                        if (fd.result().empty() == false) {
                            LoadScenario(fd.result());
                        }
                        return true;
                    }
                }
                break;
            }
            default: {
                break;
            }
        }
        return m_freeCam->UpdateEvents(event);
    }

    void Update(float dt) override { m_freeCam->Update(dt); }
    glm::mat4 GetCurrentProjectionMatrix() { return m_currentProjectionMatrix; }
    glm::mat4 GetCurrentViewMatrix() { return m_currentViewMatrix; }

   private:
    std::unique_ptr<FreeCameraMovement> m_freeCam;
    glm::mat4 m_currentProjectionMatrix{};
    glm::mat4 m_currentViewMatrix{};
};

constexpr GLfloat g_quadVertices[] = {
    -1.0f, -1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +1.0f, -1.0f,
};

constexpr GLuint g_quadIndices[] = {
    0, 1, 2, 0, 2, 3,
};

constexpr GLsizei g_quadNumIndices = 6;

bool ReadFileContent(const char* filename, uint8_t** data, size_t* length) {
    SDL_RWops* file = SDL_RWFromFile(filename, "rb");
    if (file == nullptr) {
        return false;
    }
    SDL_RWseek(file, 0, RW_SEEK_END);
    *length = SDL_RWtell(file);
    SDL_RWseek(file, 0, RW_SEEK_SET);
    *data = new uint8_t[*length];
    SDL_RWread(file, *data, *length, 1);
    SDL_RWclose(file);
    return true;
}

bool LoadShader(const char* filename, GLenum shaderType, GLuint& shaderID) {
    size_t fileDataSize = 0;
    uint8_t* fileData = nullptr;
    if (ReadFileContent(filename, &fileData, &fileDataSize)) {
        // Compile the shader.
        shaderID = glCreateShader(shaderType);
        const GLint shaderFileSize = static_cast<GLint>(fileDataSize);
        glShaderSource(shaderID, 1, reinterpret_cast<GLchar**>(&fileData), &shaderFileSize);
        glCompileShader(shaderID);

        // Print the info.
        GLint isCompiled = 0;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);
            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(shaderID, maxLength, &maxLength, &infoLog[0]);
            std::printf("%s", infoLog.data());
            glDeleteShader(shaderID);
            return false;
        }

        // Delete the file data.
        delete[] fileData;

        // Success.
        return true;
    }

    // Print the failure message.
    std::printf("Failed to load %s\n", filename);

    // Failed.
    return false;
}

bool LinkShaders(const std::vector<GLuint>& shadersID, GLuint& programID) {
    // Link the program.
    programID = glCreateProgram();
    for (auto& shaderID : shadersID) {
        if (shaderID != 0) {
            glAttachShader(programID, shaderID);
        }
    }
    glLinkProgram(programID);

    // Print the info.
    GLint isLinked = 0;
    glGetProgramiv(programID, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(programID, maxLength, &maxLength, &infoLog[0]);
        std::printf("%s", infoLog.data());
        glDeleteProgram(programID);
        return false;
    }

    // Detach the shaders.
    for (auto& shaderID : shadersID) {
        if (shaderID != 0) {
            glDetachShader(programID, shaderID);
        }
    }

    // Success.
    return true;
}

bool LoadTexture(const char* filename, bool depthTexture, bool mipmaps, bool clamp, bool linear, GLuint& textureID) {
    size_t fileDataSize = 0;
    uint8_t* fileData = nullptr;
    if (ReadFileContent(filename, &fileData, &fileDataSize)) {
        // Load the texture data.
        int32_t textureWidth, textureHeight, textureNumChannels;
        uint8_t* textureData =
            stbi_load_from_memory(fileData, fileDataSize, &textureWidth, &textureHeight, &textureNumChannels, 4);

        // Delete the file data.
        delete[] fileData;

        // Check if the texture data is valid.
        if (textureData) {
            // Create the texture.
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp ? GL_CLAMP : GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp ? GL_CLAMP : GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                            mipmaps  ? GL_LINEAR_MIPMAP_LINEAR
                            : linear ? GL_LINEAR
                                     : GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                            mipmaps  ? GL_LINEAR
                            : linear ? GL_LINEAR
                                     : GL_NEAREST);
            if (depthTexture) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, textureWidth, textureHeight, 0, GL_RED, GL_FLOAT, textureData);
            } else {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, textureWidth, textureHeight, 0, GL_RGBA,
                             GL_UNSIGNED_BYTE, textureData);
            }

            // Generate the mipmaps.
            if (mipmaps) {
                glGenerateMipmap(GL_TEXTURE_2D);
            }

            // Free the texture data.
            stbi_image_free(textureData);

            // Succeed.
            return true;
        }
    }

    // Print the failure message.
    std::printf("Failed to load %s\n", filename);

    // Failed.
    return false;
}

void CreateMeshQuad(RenderContext* renderContext, Mesh& mesh) {
    // Generate and bind the quad vertex array.
    glGenVertexArrays(1, &mesh.m_vertexArray);
    glBindVertexArray(mesh.m_vertexArray);

    // Create the quad vertex buffer.
    glGenBuffers(1, &mesh.m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quadVertices), g_quadVertices, GL_STATIC_DRAW);

    // Set the position vertex attribute.
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    // Create the quad index buffer.
    glGenBuffers(1, &mesh.m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_quadIndices), &g_quadIndices[0], GL_STATIC_DRAW);

    // Create the sub-mesh.
    SubMesh subMesh;
    subMesh.m_indexStart = 0;
    subMesh.m_indexCount = 6;
    subMesh.m_materialIndex = -1;
    mesh.m_subMeshes.push_back(subMesh);
}

void CreateMeshOBJ(const std::string& filename, Mesh& mesh) {
    // Extract the folder path.
    std::string folderPath = filename;
    std::replace(folderPath.begin(), folderPath.end(), '\\', '/');
    folderPath = folderPath.substr(0, folderPath.rfind('/'));

    // Create the OBJ reader config.
    tinyobj::ObjReaderConfig objReaderConfig;
    objReaderConfig.mtl_search_path = "./";

    // Load the OBJ file.
    tinyobj::ObjReader objReader;
    if (!objReader.ParseFromFile(filename)) {
        const std::string& errors = objReader.Error();
        if (!errors.empty()) {
            std::printf("ERROR: %s\n", objReader.Error().c_str());
        }
        return;
    }

    // Print warnings if not empty.
    const std::string& warnings = objReader.Warning();
    if (!warnings.empty()) {
        std::printf("WARNING: %s\n", warnings.c_str());
    }

    // Create each material.
    const std::vector<tinyobj::material_t>& materials = objReader.GetMaterials();
    if (!materials.empty()) {
        mesh.m_materials.resize(materials.size());
        for (size_t i = 0; i < materials.size(); ++i) {
            Material& meshMaterial = mesh.m_materials[i];
            const tinyobj::material_t& objMaterial = materials[i];
            meshMaterial.m_baseColor =
                glm::vec3(objMaterial.diffuse[0], objMaterial.diffuse[1], objMaterial.diffuse[2]);
            if (!objMaterial.diffuse_texname.empty()) {
                std::string diffuseTextureFullPath = folderPath + '/' + objMaterial.diffuse_texname;
                std::replace(diffuseTextureFullPath.begin(), diffuseTextureFullPath.end(), '\\', '/');
                LoadTexture(diffuseTextureFullPath.c_str(), false, true, false, true, meshMaterial.m_diffuseTexture);
            }
        }
    } else {
        Material material;
        mesh.m_materials.push_back(material);
    }

    // Vertex struct.
    struct Vertex {
        glm::vec3 m_position;
        glm::vec3 m_normal;
        glm::vec2 m_uv;
    };

    // Create the array of vertices for each material.
    std::vector<std::vector<Vertex>> materialVertices;
    materialVertices.resize(mesh.m_materials.size());

    // Create each sub-mesh.
    const tinyobj::attrib_t& attrib = objReader.GetAttrib();
    const std::vector<tinyobj::shape_t>& shapes = objReader.GetShapes();
    for (size_t i = 0; i < shapes.size(); ++i) {
        size_t indexOffset = 0;
        for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); ++f) {
            const size_t faceVertexCount = shapes[i].mesh.num_face_vertices[f];
            const int32_t faceMaterialIndex = shapes[i].mesh.material_ids[f];
            std::vector<Vertex>& vertices = materialVertices[(faceMaterialIndex >= 0) ? faceMaterialIndex : 0];
            for (size_t v = 0; v < faceVertexCount; ++v) {
                Vertex vertex;
                const tinyobj::index_t& index = shapes[i].mesh.indices[indexOffset + v];
                vertex.m_position.x = attrib.vertices[3 * index.vertex_index + 0];
                vertex.m_position.y = attrib.vertices[3 * index.vertex_index + 1];
                vertex.m_position.z = attrib.vertices[3 * index.vertex_index + 2];
                if (!attrib.normals.empty() && (index.normal_index >= 0)) {
                    vertex.m_normal.x = attrib.normals[3 * index.normal_index + 0];
                    vertex.m_normal.y = attrib.normals[3 * index.normal_index + 1];
                    vertex.m_normal.z = attrib.normals[3 * index.normal_index + 2];
                }
                if (!attrib.texcoords.empty() && (index.texcoord_index >= 0)) {
                    vertex.m_uv.x = attrib.texcoords[2 * index.texcoord_index + 0];
                    vertex.m_uv.y = attrib.texcoords[2 * index.texcoord_index + 1];
                }
                vertices.push_back(vertex);
            }
            indexOffset += faceVertexCount;
        }
    }

    // Compute the vertex count.
    size_t vertexCount = 0;
    for (size_t i = 0; i < materialVertices.size(); ++i) {
        vertexCount += materialVertices[i].size();
    }

    // Create the vertex data.
    const size_t vertexDataSize = vertexCount * sizeof(Vertex);
    auto vertexData = std::make_unique<uint8_t[]>(vertexDataSize);
    auto* vertexDataIterator = vertexData.get();
    for (size_t i = 0; i < materialVertices.size(); ++i) {
        for (size_t v = 0; v < materialVertices[i].size(); ++v) {
            std::memcpy(vertexDataIterator, &materialVertices[i][v].m_position, sizeof(glm::vec3));
            vertexDataIterator += sizeof(glm::vec3);
            std::memcpy(vertexDataIterator, &materialVertices[i][v].m_normal, sizeof(glm::vec3));
            vertexDataIterator += sizeof(glm::vec3);
            std::memcpy(vertexDataIterator, &materialVertices[i][v].m_uv, sizeof(glm::vec2));
            vertexDataIterator += sizeof(glm::vec2);
        }
    }

    // Create the index data.
    const size_t indexDataSize = vertexCount * sizeof(uint32_t);
    auto indexData = std::make_unique<uint32_t[]>(vertexCount);
    for (size_t i = 0; i < vertexCount; i += 3) {
        indexData[i] = i;
        indexData[i + 1] = i + 2;
        indexData[i + 2] = i + 1;
    }

    // Generate and bind the vertex array.
    glGenVertexArrays(1, &mesh.m_vertexArray);
    glBindVertexArray(mesh.m_vertexArray);

    // Create the vertex buffer.
    glGenBuffers(1, &mesh.m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vertexData.get(), GL_STATIC_DRAW);

    // Set the position vertex attribute.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    // Set the normal vertex attribute.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Set the uv vertex attribute.
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void*>(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // Create the index buffer.
    glGenBuffers(1, &mesh.m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, indexData.get(), GL_STATIC_DRAW);

    // Create and add the sub-meshes.
    uint32_t indexStart = 0;
    for (size_t i = 0; i < materialVertices.size(); ++i) {
        if (!materialVertices[i].empty()) {
            // Create and add the sub-mesh.
            SubMesh subMesh;
            subMesh.m_materialIndex = i;
            subMesh.m_indexStart = indexStart;
            subMesh.m_indexCount = materialVertices[i].size();
            mesh.m_subMeshes.push_back(subMesh);

            // Offset the index start.
            indexStart += materialVertices[i].size();
        }
    }
}

void RenderMesh(RenderContext* renderContext, Mesh& mesh) {
    // Bind the mesh buffers.
    glBindVertexArray(mesh.m_vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.m_vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.m_indexBuffer);

    // Render each submesh of the mesh.
    for (size_t i = 0; i < mesh.m_subMeshes.size(); ++i) {
        // Get the submesh.
        const SubMesh& submesh = mesh.m_subMeshes[i];

        if (submesh.m_materialIndex >= 0) {
            // Get the matrial.
            const Material& material = mesh.m_materials[submesh.m_materialIndex];

            // Set the base color shader parameter.
            glUniform3fv(renderContext->m_shaderMesh.m_locBaseColor, 1, &material.m_baseColor[0]);

            // Set the diffuse texture shader parameter.
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(renderContext->m_shaderMesh.m_locDiffuseTexture, 0);
            if (material.m_diffuseTexture != 0) {
                glBindTexture(GL_TEXTURE_2D, material.m_diffuseTexture);
            } else {
                glBindTexture(GL_TEXTURE_2D, renderContext->m_whiteTexture);
            }
        }

        // Draw the submesh.
        glDrawElements(GL_TRIANGLES, submesh.m_indexCount, GL_UNSIGNED_INT,
                       reinterpret_cast<void*>(submesh.m_indexStart * sizeof(GLuint)));
    }
}

void DeleteMesh(Mesh& mesh) {
    glDeleteVertexArrays(1, &mesh.m_vertexArray);
    glDeleteBuffers(1, &mesh.m_vertexBuffer);
    glDeleteBuffers(1, &mesh.m_indexBuffer);
    for (size_t i = 0; i < mesh.m_materials.size(); ++i) {
        glDeleteTextures(1, &mesh.m_materials[i].m_diffuseTexture);
    }
    mesh.m_subMeshes.clear();
}

void CreateRenderTarget(uint32_t width, uint32_t height, uint32_t format, bool linear, RenderTarget& output) {
    // Create the frame buffer object.
    glGenFramebuffers(1, &output.m_fbo);

    // Set the render target width and height.
    output.m_width = width;
    output.m_height = height;

    // Create the texture.
    glCreateTextures(GL_TEXTURE_2D, 1, &output.m_texture);
    glTextureParameteri(output.m_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(output.m_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(output.m_texture, GL_TEXTURE_MIN_FILTER, linear ? GL_LINEAR : GL_NEAREST);
    glTextureParameteri(output.m_texture, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);
    glTextureStorage2D(output.m_texture, 1, format, output.m_width, output.m_height);

    // Set the depth frame buffer.
    glBindFramebuffer(GL_FRAMEBUFFER, output.m_fbo);

    // Attach the texture to the frame buffer.
    glNamedFramebufferTexture(output.m_fbo, GL_COLOR_ATTACHMENT0, output.m_texture, 0);
    const GLuint attachments[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, &attachments[0]);
    const GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::printf("Framebuffer not complete, error: %d", framebufferStatus);
        if (framebufferStatus == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) {
            std::printf("Framebuffer attachments missing");
        }
    }
}

void Init(RenderContext* renderContext) {

    renderContext->m_glDebugOutput = std::make_unique<ml::Reprojection::Performance::GLDebugOutput>();

    InitReprojection(renderContext);

    // Set the default draw parameters.
    glFrontFace(GL_CW);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_FRAMEBUFFER_SRGB);

    // Load the shaders.
    LoadShader("../Content/Shaders/Mesh.vs", GL_VERTEX_SHADER, renderContext->m_shaderMesh.m_vertexShader);
    LoadShader("../Content/Shaders/Mesh.fs", GL_FRAGMENT_SHADER, renderContext->m_shaderMesh.m_fragmentShader);
    LoadShader("../../ReprojectionLib/Shaders/FullscreenQuad.vs", GL_VERTEX_SHADER,
               renderContext->m_fullscreenQuadVertexShader);
    LoadShader("../../ReprojectionLib/Shaders/TextureSampling.fs", GL_FRAGMENT_SHADER,
               renderContext->m_shaderTextureSampling.m_fragmentShader);

    // Link the shaders.
    LinkShaders({renderContext->m_shaderMesh.m_vertexShader, renderContext->m_shaderMesh.m_fragmentShader},
                renderContext->m_shaderMesh.m_program);
    LinkShaders({renderContext->m_fullscreenQuadVertexShader, renderContext->m_shaderTextureSampling.m_fragmentShader},
                renderContext->m_shaderTextureSampling.m_program);

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
    CreateMeshOBJ("../Content/Models/Statuette/Statuette.obj", renderContext->m_mesh);

    // Create the white texture.
    const uint8_t whiteTextureData[4] = {255, 255, 255, 255};
    glGenTextures(1, &renderContext->m_whiteTexture);
    glBindTexture(GL_TEXTURE_2D, renderContext->m_whiteTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whiteTextureData);

    // Create the color down-sampling and up-sampling render targets.
    CreateRenderTarget(renderContext->m_screenWidth / 2, renderContext->m_screenHeight / 2, GL_RGBA8, true,
                       renderContext->m_colorDownSamplingRenderTarget);
    CreateRenderTarget(renderContext->m_screenWidth, renderContext->m_screenHeight, GL_RGBA8, true,
                       renderContext->m_colorUpSamplingRenderTarget);

    // Create the depth down-sampling and up-sampling render targets.
    size_t depthDownSamplingRenderTargetIndex = 0;

    for (uint32_t downscaleFactor = 2; downscaleFactor <= 8; downscaleFactor *= 2) {
        for (size_t i = 0; i < 2; ++i) {
            CreateRenderTarget(renderContext->m_screenWidth / downscaleFactor,
                               renderContext->m_screenHeight / downscaleFactor, GL_R8, false,
                               renderContext->m_depthDownSamplingRenderTargets[depthDownSamplingRenderTargetIndex++]);
        }
    }
    CreateRenderTarget(renderContext->m_screenWidth, renderContext->m_screenHeight, GL_R8, true,
                       renderContext->m_depthUpSamplingRenderTarget);

    // Set the default camera transformation.
    renderContext->m_camera.Move({0, 0, 2});
}

void DeleteAppMode(RenderContext* renderContext) { renderContext->m_appModeImpl.reset(); }

void Shutdown(RenderContext* renderContext) {
    ml::Reprojection::Shutdown();
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
    glDeleteTextures(1, &renderContext->m_currentFrameDepthTexture);
    glDeleteFramebuffers(1, &renderContext->m_colorDownSamplingRenderTarget.m_fbo);
    glDeleteTextures(1, &renderContext->m_colorDownSamplingRenderTarget.m_texture);
    glDeleteFramebuffers(1, &renderContext->m_colorUpSamplingRenderTarget.m_fbo);
    glDeleteTextures(1, &renderContext->m_colorUpSamplingRenderTarget.m_texture);
    for (size_t i = 0; i < 6; ++i) {
        glDeleteFramebuffers(1, &renderContext->m_depthDownSamplingRenderTargets[i].m_fbo);
        glDeleteTextures(1, &renderContext->m_depthDownSamplingRenderTargets[i].m_texture);
    }
    glDeleteFramebuffers(1, &renderContext->m_depthUpSamplingRenderTarget.m_fbo);
    glDeleteTextures(1, &renderContext->m_depthUpSamplingRenderTarget.m_texture);

    renderContext->m_glDebugOutput.reset();
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

    // GL errors
    renderContext->m_glDebugOutput->RenderGUI();

    // Render.
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Enable the SRGB to have proper scene colors.
    glEnable(GL_FRAMEBUFFER_SRGB);
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

        if (renderContext.m_mustReloadReprojection) {
            ReloadReprojection(&renderContext);
            renderContext.m_mustReloadReprojection = false;
        }

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
