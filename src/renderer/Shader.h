#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace forge {

/**
 * @brief GLSL shader program (vertex + fragment) with a cached uniform API.
 *
 * Loads and compiles two GLSL source files, links them into a program, and
 * provides typed setters for every uniform type used by the engine.
 * Uniform locations are cached after the first lookup.
 *
 * @code
 * forge::Shader shader("assets/shaders/sprite.vert", "assets/shaders/sprite.frag");
 * shader.bind();
 * shader.setMat4("u_ViewProjection", camera.getViewProjection());
 * shader.setInt ("u_Texture", 0);
 * @endcode
 */
class Shader {
public:
    /**
     * @brief Compiles and links a shader program from two GLSL files.
     * @param vertPath Path to the vertex shader source file.
     * @param fragPath Path to the fragment shader source file.
     * @throws std::runtime_error on compile or link failure.
     */
    Shader(const std::string& vertPath, const std::string& fragPath);
    ~Shader();

    Shader(const Shader&)            = delete;
    Shader& operator=(const Shader&) = delete;

    /** @brief Binds this shader program for subsequent draw calls. */
    void bind()   const;

    /** @brief Unbinds the shader program (binds program 0). */
    void unbind() const;

    /** @brief Sets an integer uniform (e.g. sampler slot). */
    void setInt  (const std::string& name, int value);
    /** @brief Sets a float uniform. */
    void setFloat(const std::string& name, float value);
    /** @brief Sets a vec2 uniform. */
    void setVec2 (const std::string& name, const glm::vec2& v);
    /** @brief Sets a vec3 uniform. */
    void setVec3 (const std::string& name, const glm::vec3& v);
    /** @brief Sets a vec4 uniform. */
    void setVec4 (const std::string& name, const glm::vec4& v);
    /** @brief Sets a mat4 uniform. */
    void setMat4 (const std::string& name, const glm::mat4& m);

private:
    uint32_t m_id = 0;
    std::unordered_map<std::string, int> m_uniformCache;

    int getUniformLocation(const std::string& name);
    static uint32_t compileShader(uint32_t type, const std::string& source);
};

} // namespace forge
