#pragma once
#include <string>
#include <cstdint>

namespace forge {

/**
 * @brief OpenGL texture (2D) with three construction paths.
 *
 * - **From file** — loads a PNG, JPG, or any format supported by stb_image.
 * - **Solid colour** — fills every pixel with a single RGBA value (useful for UI/debug).
 * - **Raw pixels** — uploads an array of packed RGBA uint32_t values directly.
 *
 * The texture is uploaded to the GPU in the constructor and is available for
 * rendering immediately. Non-copyable; move if ownership transfer is needed.
 *
 * @code
 * // Load from file:
 * forge::Texture tex("assets/player.png");
 *
 * // 2×2 solid red:
 * forge::Texture red(2, 2, 0xFF0000FFu);
 *
 * // Custom pixel data:
 * std::vector<uint32_t> pixels = { ... };
 * forge::Texture custom(64, 64, pixels.data());
 * @endcode
 */
class Texture {
public:
    /**
     * @brief Loads a texture from an image file.
     * @param path File path (PNG, JPG, BMP, etc.).
     * @throws std::runtime_error if the file cannot be loaded.
     */
    explicit Texture(const std::string& path);

    /**
     * @brief Creates a solid-colour texture of the given size.
     * @param width     Width in pixels.
     * @param height    Height in pixels.
     * @param colorRGBA Packed RGBA colour (0xRRGGBBAA), applied to every pixel.
     */
    Texture(int width, int height, uint32_t colorRGBA);

    /**
     * @brief Uploads raw RGBA pixel data.
     * @param width  Width in pixels.
     * @param height Height in pixels.
     * @param pixels Array of @p width * @p height packed RGBA uint32_t values (row-major).
     */
    Texture(int width, int height, const uint32_t* pixels);

    ~Texture();

    Texture(const Texture&)            = delete;
    Texture& operator=(const Texture&) = delete;

    /**
     * @brief Binds the texture to the given sampler slot.
     * @param slot OpenGL texture unit index (0–15).
     */
    void bind(uint32_t slot = 0) const;

    /** @brief Unbinds the texture from its current slot. */
    void unbind() const;

    int width()  const { return m_width; }  ///< Width in pixels.
    int height() const { return m_height; } ///< Height in pixels.

private:
    uint32_t m_id     = 0;
    int      m_width  = 0;
    int      m_height = 0;

    void upload(const unsigned char* data, int w, int h, int channels);
};

} // namespace forge
