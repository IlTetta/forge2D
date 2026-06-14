#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

namespace forge {

class Texture;

class TileMap {
public:
    // ---- Programmatic setup ------------------------------------------------

    // Set the tileset. tiles are arranged left-to-right in a single row
    // or a grid of `cols` columns; row 0 = top of the image.
    void setTileset(const Texture* texture, int tileWidth, int tileHeight, int cols);

    // Resize the map and fill every cell with 0 (empty).
    void resize(int mapWidth, int mapHeight);

    // Set a tile at grid (col, row) where (0,0) = top-left. gid=0 means empty.
    // GIDs are 1-based: gid=1 is the first tile (col=0, row=0) of the tileset.
    void setTile(int col, int row, int gid);
    int  getTile(int col, int row) const;

    // ---- Load from Tiled JSON export ---------------------------------------

    // Loads map and tilesets; textureBasePath is prepended to image paths in JSON.
    bool loadFromFile(const std::string& jsonPath,
                      const std::string& textureBasePath = "");

    // ---- Render ------------------------------------------------------------

    // origin = world-space position of the bottom-left corner of the map.
    void render(const glm::vec2& origin = {0.f, 0.f}) const;

    int tileWidth()  const { return m_tileW; }
    int tileHeight() const { return m_tileH; }
    int mapWidth()   const { return m_mapW; }
    int mapHeight()  const { return m_mapH; }

private:
    struct Tileset {
        const Texture*              texture     = nullptr;
        std::unique_ptr<Texture>    ownedTex;   // only set when loaded from file
        int firstGid = 1;
        int tileW    = 32, tileH = 32;
        int cols     = 1,  rows  = 1;

        glm::vec4 getUV(int gid) const;
    };

    struct Layer {
        std::string      name;
        std::vector<int> tiles; // size = mapW * mapH; 0 = empty
    };

    const Tileset* tilesetForGid(int gid) const;

    std::vector<Tileset> m_tilesets;
    std::vector<Layer>   m_layers;
    int m_tileW = 32, m_tileH = 32;
    int m_mapW  = 0,  m_mapH  = 0;
};

} // namespace forge
