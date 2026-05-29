#pragma once
#include <string>
#include <cstdint>

namespace forge {

class Texture {
public:
    explicit Texture(const std::string& path);
    Texture(int width, int height, uint32_t colorRGBA);
    ~Texture();

    Texture(const Texture&)            = delete;
    Texture& operator=(const Texture&) = delete;

    void bind(uint32_t slot = 0) const;
    void unbind() const;

    int width()  const { return m_width; }
    int height() const { return m_height; }

private:
    uint32_t m_id     = 0;
    int      m_width  = 0;
    int      m_height = 0;

    void upload(const unsigned char* data, int w, int h, int channels);
};

} // namespace forge
