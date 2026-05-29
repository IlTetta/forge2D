#include "Shader.h"
#include "core/Log.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace forge {

static std::string readFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open())
        throw std::runtime_error("Cannot open shader file: " + path);
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

uint32_t Shader::compileShader(uint32_t type, const std::string& source) {
    uint32_t id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int ok;
    glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(id, 512, nullptr, log);
        glDeleteShader(id);
        throw std::runtime_error(std::string("Shader compile error:\n") + log);
    }
    return id;
}

Shader::Shader(const std::string& vertPath, const std::string& fragPath) {
    uint32_t vert = compileShader(GL_VERTEX_SHADER,   readFile(vertPath));
    uint32_t frag = compileShader(GL_FRAGMENT_SHADER, readFile(fragPath));

    m_id = glCreateProgram();
    glAttachShader(m_id, vert);
    glAttachShader(m_id, frag);
    glLinkProgram(m_id);

    int ok;
    glGetProgramiv(m_id, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetProgramInfoLog(m_id, 512, nullptr, log);
        glDeleteProgram(m_id);
        glDeleteShader(vert);
        glDeleteShader(frag);
        throw std::runtime_error(std::string("Shader link error:\n") + log);
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
    FG_TRACE("Shader linked: vert=%s  frag=%s", vertPath.c_str(), fragPath.c_str());
}

Shader::~Shader() {
    glDeleteProgram(m_id);
}

void Shader::bind()   const { glUseProgram(m_id); }
void Shader::unbind() const { glUseProgram(0); }

int Shader::getUniformLocation(const std::string& name) {
    auto it = m_uniformCache.find(name);
    if (it != m_uniformCache.end()) return it->second;
    int loc = glGetUniformLocation(m_id, name.c_str());
    if (loc == -1) FG_WARN("Uniform '%s' not found in shader", name.c_str());
    return m_uniformCache[name] = loc;
}

void Shader::setInt  (const std::string& n, int v)           { glUniform1i (getUniformLocation(n), v); }
void Shader::setFloat(const std::string& n, float v)         { glUniform1f (getUniformLocation(n), v); }
void Shader::setVec2 (const std::string& n, const glm::vec2& v) { glUniform2fv(getUniformLocation(n), 1, glm::value_ptr(v)); }
void Shader::setVec3 (const std::string& n, const glm::vec3& v) { glUniform3fv(getUniformLocation(n), 1, glm::value_ptr(v)); }
void Shader::setVec4 (const std::string& n, const glm::vec4& v) { glUniform4fv(getUniformLocation(n), 1, glm::value_ptr(v)); }
void Shader::setMat4 (const std::string& n, const glm::mat4& m) { glUniformMatrix4fv(getUniformLocation(n), 1, GL_FALSE, glm::value_ptr(m)); }

} // namespace forge
