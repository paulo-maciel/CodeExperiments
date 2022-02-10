#pragma once

#include <GL/glew.h>
#include <SDL.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#include <vector>
#include <memory>


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

enum AppMode { FreeCamera, NumModes };
class AppModeImplementation;

struct RenderContext {
    uint32_t m_screenWidth = 0;
    uint32_t m_screenHeight = 0;
    GLuint m_whiteTexture = 0;
    Mesh m_quad;
    Mesh m_mesh;
    Camera m_camera;
    AppMode m_appMode = AppMode::FreeCamera;
    std::unique_ptr<AppModeImplementation> m_appModeImpl;
    GLuint m_fullscreenQuadVertexShader = 0;
    ShaderTextureSampling m_shaderTextureSampling;
    ShaderMesh m_shaderMesh;
    GLuint m_currentFrameColorTexture = 0;

    float m_positionOffset[3] = {};
    float m_rotationOffset[3] = {};
    int32_t m_frameDelay = 0;

    RenderTarget m_colorRenderTarget;

    bool m_wireframe = false;
    bool m_cullBackground = true;
};

void DeleteAppMode(RenderContext* renderContext);

std::string GetAppModeStr(AppMode am);

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
    FreeCameraMovement(RenderContext* rc);

    void RenderGUI() override;

    bool UpdateEvents(const SDL_Event& event) override;


    void Update(float dt) override;

    void FromViewMatrix(glm::mat4 viewMat);

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
std::string KeyframeRootNameToString(size_t index);

class AppModeImplementationImpl final : public AppModeImplementation {
   public:
    AppModeImplementationImpl(RenderContext* rc);

    void LoadScenario(const std::string& path);

    void RenderGUI() override;

    bool UpdateEvents(const SDL_Event& event) override;

    void Update(float dt) override;
    glm::mat4 GetCurrentProjectionMatrix();
    glm::mat4 GetCurrentViewMatrix();

   private:
    std::unique_ptr<FreeCameraMovement> m_freeCam;
    glm::mat4 m_currentProjectionMatrix{};
    glm::mat4 m_currentViewMatrix{};
};

void SetAppMode(RenderContext* renderContext, AppMode newAppMode);

constexpr GLfloat g_quadVertices[] = {
    -1.0f, -1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +1.0f, -1.0f,
};

constexpr GLuint g_quadIndices[] = {
    0, 1, 2, 0, 2, 3,
};

constexpr GLsizei g_quadNumIndices = 6;

bool ReadFileContent(const char* filename, uint8_t** data, size_t* length);

bool LoadShader(const char* filename, GLenum shaderType, GLuint& shaderID);

bool LinkShaders(const std::vector<GLuint>& shadersID, GLuint& programID);

bool LoadTexture(const char* filename, bool depthTexture, bool mipmaps, bool clamp, bool linear, GLuint& textureID);

void CreateMeshQuad(RenderContext* renderContext, Mesh& mesh);

void CreateMeshOBJ(const std::string& filename, Mesh& mesh);

void RenderMesh(RenderContext* renderContext, Mesh& mesh);

void DeleteMesh(Mesh& mesh);

void CreateRenderTarget(uint32_t width, uint32_t height, uint32_t format, bool linear, RenderTarget& output);
