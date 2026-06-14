#include "Texture.h"
#include "core/Log.h"
#include <glad/glad.h>
#include <stb_image.h>
#include <vector>
#include <stdexcept>

namespace forge {

void Texture::upload(const unsigned char* data, int w, int h, int channels) {
    m_width  = w;
    m_height = h;

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum fmt = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(const std::string& path) {
    stbi_set_flip_vertically_on_load(true);
    int w, h, ch;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 0);
    if (!data)
        throw std::runtime_error("stbi_load failed: " + path);
    upload(data, w, h, ch);
    stbi_image_free(data);
    FG_TRACE("Texture loaded: %s  (%dx%d  ch=%d)", path.c_str(), w, h, ch);
}

Texture::Texture(int width, int height, uint32_t colorRGBA) {
    std::vector<uint32_t> pixels(width * height, colorRGBA);
    upload(reinterpret_cast<unsigned char*>(pixels.data()), width, height, 4);
}

Texture::Texture(int width, int height, const uint32_t* pixels) {
    upload(reinterpret_cast<const unsigned char*>(pixels), width, height, 4);
}

Texture::~Texture() {
    glDeleteTextures(1, &m_id);
}

void Texture::bind(uint32_t slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace forge
