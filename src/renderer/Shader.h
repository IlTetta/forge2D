#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace forge {

class Shader {
public:
    Shader(const std::string& vertPath, const std::string& fragPath);
    ~Shader();

    Shader(const Shader&)            = delete;
    Shader& operator=(const Shader&) = delete;

    void bind()   const;
    void unbind() const;

    void setInt  (const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec2 (const std::string& name, const glm::vec2& v);
    void setVec3 (const std::string& name, const glm::vec3& v);
    void setVec4 (const std::string& name, const glm::vec4& v);
    void setMat4 (const std::string& name, const glm::mat4& m);

private:
    uint32_t m_id = 0;
    std::unordered_map<std::string, int> m_uniformCache;

    int      getUniformLocation(const std::string& name);
    static uint32_t compileShader(uint32_t type, const std::string& source);
};

} // namespace forge
