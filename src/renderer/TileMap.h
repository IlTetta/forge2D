#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

namespace forge {

class Texture;

/**
 * @brief Tile-based 2D map with support for programmatic setup and Tiled JSON export.
 *
 * A TileMap is made up of one or more tile layers. Each cell stores a GID
 * (global tile ID): 0 = empty, 1 = first tile of the first tileset, and so on.
 *
 * **Programmatic workflow:**
 * @code
 * forge::TileMap map;
 * map.setTileset(tileset.get(), 32, 32, 8); // 8 columns of 32×32 tiles
 * map.resize(20, 15);                        // 20 columns × 15 rows
 * map.setTile(3, 2, 1);                      // column 3, row 2 → first tile
 * map.render({-320.f, -240.f});              // bottom-left corner of the map
 * @endcode
 *
 * **Tiled JSON workflow:**
 * @code
 * forge::TileMap map;
 * if (!map.loadFromFile("assets/level1.json", "assets/"))
 *     FG_ERROR("Failed to load map");
 * map.render({0.f, 0.f});
 * @endcode
 */
class TileMap {
public:
    // ---- Programmatic setup ------------------------------------------------

    /**
     * @brief Sets the tileset texture and its grid layout.
     * @param texture    The tileset image (must outlive this TileMap).
     * @param tileWidth  Width of one tile in pixels.
     * @param tileHeight Height of one tile in pixels.
     * @param cols       Number of tile columns in the texture.
     */
    void setTileset(const Texture* texture, int tileWidth, int tileHeight, int cols);

    /**
     * @brief Resizes the map and fills every cell with 0 (empty).
     * @param mapWidth  Number of tile columns.
     * @param mapHeight Number of tile rows.
     */
    void resize(int mapWidth, int mapHeight);

    /**
     * @brief Sets the tile at grid position (col, row).
     *
     * Origin (0, 0) is the top-left cell. GIDs are 1-based; 0 = empty.
     */
    void setTile(int col, int row, int gid);

    /** @brief Returns the GID at (col, row), or 0 if out of bounds. */
    int getTile(int col, int row) const;

    // ---- Load from Tiled JSON export ---------------------------------------

    /**
     * @brief Loads the map from a Tiled JSON export file.
     *
     * Image paths found in the JSON are prefixed with @p textureBasePath.
     * Requires FORGE_HAS_JSON (nlohmann/json) to be defined at compile time.
     *
     * @param jsonPath        Path to the exported .json file.
     * @param textureBasePath Directory prefix prepended to tileset image paths.
     * @return true on success, false if the file could not be parsed.
     */
    bool loadFromFile(const std::string& jsonPath,
                      const std::string& textureBasePath = "");

    // ---- Render ------------------------------------------------------------

    /**
     * @brief Draws all visible layers of the map.
     * @param origin World-space position of the **bottom-left** corner of the map.
     */
    void render(const glm::vec2& origin = {0.f, 0.f}) const;

    int tileWidth()  const { return m_tileW; } ///< Width of one tile in pixels.
    int tileHeight() const { return m_tileH; } ///< Height of one tile in pixels.
    int mapWidth()   const { return m_mapW; }  ///< Map width in tiles.
    int mapHeight()  const { return m_mapH; }  ///< Map height in tiles.

private:
    struct Tileset {
        const Texture*           texture  = nullptr;
        std::unique_ptr<Texture> ownedTex;       ///< Owned when loaded from file.
        int firstGid = 1;
        int tileW    = 32, tileH = 32;
        int cols     = 1,  rows  = 1;

        glm::vec4 getUV(int gid) const;
    };

    struct Layer {
        std::string      name;
        std::vector<int> tiles; ///< Flat array, size = mapW × mapH; 0 = empty.
    };

    const Tileset* tilesetForGid(int gid) const;

    std::vector<Tileset> m_tilesets;
    std::vector<Layer>   m_layers;
    int m_tileW = 32, m_tileH = 32;
    int m_mapW  = 0,  m_mapH  = 0;
};

} // namespace forge
