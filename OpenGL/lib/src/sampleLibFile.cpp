#include "../include/sampleLibFile.h"

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

using namespace std;

void DeleteAppMode(RenderContext* renderContext) { 
  renderContext->m_appModeImpl.reset();
}

std::string GetAppModeStr(AppMode am) {
    switch (am) {
        case AppMode::FreeCamera:
            return "Free camera movement";
        default:
            return "Invalid";
    }
}


FreeCameraMovement::FreeCameraMovement(RenderContext* rc) : AppModeImplementation(rc) {
  m_cameraAngles = glm::degrees(glm::eulerAngles(m_renderContext->m_camera.m_rotation));
}

void FreeCameraMovement::RenderGUI() {
    if (m_renderContext->m_appMode == AppMode::FreeCamera) {
        ImGui::Text(
            "Sample text1 here"
            "Sample text here (%.2f)", m_movingSpeed);
    }
}

bool FreeCameraMovement::UpdateEvents(const SDL_Event& event) {
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

void FreeCameraMovement::Update(float dt) {
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

void FreeCameraMovement::FromViewMatrix(glm::mat4 viewMat) {
  glm::mat4 camMat = glm::inverse(viewMat);
  m_renderContext->m_camera.m_position = camMat[3];
  m_renderContext->m_camera.m_rotation = glm::quat_cast(camMat);
  m_cameraAngles = glm::degrees(glm::eulerAngles(m_renderContext->m_camera.m_rotation));
}

std::string KeyframeRootNameToString(size_t index) { 
  return "Key" + std::to_string(index); 
}


AppModeImplementationImpl::AppModeImplementationImpl(RenderContext* rc) : AppModeImplementation(rc) {
    m_freeCam = std::make_unique<FreeCameraMovement>(rc);
    LoadScenario("Capture/Scenario1");
}

void AppModeImplementationImpl::LoadScenario(const std::string& path) {
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
  LoadTexture((path + '/' + "ColorDump0.bmp").c_str(), false, false, true, true,
              m_renderContext->m_currentFrameColorTexture);
}

void AppModeImplementationImpl::RenderGUI() {
  m_freeCam->RenderGUI();
  ImGui::Text("[L] Load scenario");

  // Viewer UI.
  constexpr uint32_t windowHeight = 390;
  glm::vec2 windowPos = {10.0f, static_cast<float>(m_renderContext->m_screenHeight - windowHeight - 45)};
  glm::vec2 windowSize = {490.0f, static_cast<float>(windowHeight)};
  ImGui::SetNextWindowPos({windowPos.x, windowPos.y}, ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize({windowSize.x, windowSize.y}, ImGuiCond_FirstUseEver);
  ImGui::Begin("Setup");
  ImGui::Text("SOURCE SETTINGS");
  constexpr const char* colorDownSampleListboxItems[] = {"Disabled", "Bilinear"};


  ImGui::Separator();
  ImGui::Text("Settings");
  constexpr const char* algorithmListboxItems[] = {"Algo 1", "Algo 2"};
  static int item_current = 0;
  ImGui::Combo("Algorithm", &item_current, algorithmListboxItems, 3);

  ImGui::Separator();
  ImGui::Text("TARGET SETTINGS");

  ImGui::Separator();
  ImGui::Text("LAG SETTINGS");
  ImGui::SliderFloat3("Position Offset", m_renderContext->m_positionOffset, -10.0f, 10.0f);
  ImGui::SliderFloat3("Rotation Offset", m_renderContext->m_rotationOffset, -180.0, 180.0f);
  ImGui::SliderInt("Frame Delay", &m_renderContext->m_frameDelay, 0, 10);
  ImGui::End();
}

bool AppModeImplementationImpl::UpdateEvents(const SDL_Event& event) {
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

void AppModeImplementationImpl::Update(float dt)  { 
  m_freeCam->Update(dt); 
}

glm::mat4 AppModeImplementationImpl::GetCurrentProjectionMatrix() { 
  return m_currentProjectionMatrix; 
}

glm::mat4 AppModeImplementationImpl::GetCurrentViewMatrix() { 
  return m_currentViewMatrix; 
}


void SetAppMode(RenderContext* renderContext, AppMode newAppMode) {
  if (renderContext->m_appMode == newAppMode && renderContext->m_appModeImpl != nullptr) {
      return;
  }

  DeleteAppMode(renderContext);

  switch (newAppMode) {
      case AppMode::FreeCamera: {
          renderContext->m_appModeImpl = std::make_unique<FreeCameraMovement>(renderContext);
          break;
      }
    default:
      break;
  }
}

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
