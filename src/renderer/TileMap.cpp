#include "TileMap.h"
#include "Texture.h"
#include "Renderer2D.h"
#include "../core/Log.h"

#ifdef FORGE_HAS_JSON
#include <nlohmann/json.hpp>
#include <fstream>
#endif

namespace forge {

// ---- Tileset ---------------------------------------------------------------

glm::vec4 TileMap::Tileset::getUV(int gid) const {
    int local = gid - firstGid;         // 0-indexed tile within this tileset
    int col   = local % cols;
    int row   = local / cols;

    // UV formula consistent with stbi_set_flip_vertically_on_load(true):
    //   row 0 of the image (top visually) maps to UV v in [1-fh, 1].
    float fw = 1.f / (float)cols;
    float fh = 1.f / (float)rows;
    return { col * fw,       1.f - (row + 1) * fh,
            (col + 1) * fw,  1.f -  row      * fh };
}

// ---- Programmatic setup ----------------------------------------------------

void TileMap::setTileset(const Texture* texture, int tileWidth, int tileHeight,
                          int cols)
{
    m_tileW = tileWidth;
    m_tileH = tileHeight;

    Tileset ts;
    ts.texture  = texture;
    ts.firstGid = 1;
    ts.tileW    = tileWidth;
    ts.tileH    = tileHeight;
    ts.cols     = cols;
    ts.rows     = texture ? texture->height() / tileHeight : 1;
    m_tilesets.clear();
    m_tilesets.push_back(std::move(ts));
}

void TileMap::resize(int mapWidth, int mapHeight) {
    m_mapW = mapWidth;
    m_mapH = mapHeight;
    if (m_layers.empty()) m_layers.push_back({"Layer", {}});
    m_layers[0].tiles.assign(mapWidth * mapHeight, 0);
}

void TileMap::setTile(int col, int row, int gid) {
    if (m_layers.empty() || col < 0 || col >= m_mapW || row < 0 || row >= m_mapH) return;
    m_layers[0].tiles[row * m_mapW + col] = gid;
}

int TileMap::getTile(int col, int row) const {
    if (m_layers.empty() || col < 0 || col >= m_mapW || row < 0 || row >= m_mapH) return 0;
    return m_layers[0].tiles[row * m_mapW + col];
}

// ---- Load from Tiled JSON --------------------------------------------------

bool TileMap::loadFromFile(const std::string& jsonPath,
                            const std::string& textureBasePath)
{
#ifndef FORGE_HAS_JSON
    FG_WARN("TileMap::loadFromFile: built without JSON support (FORGE_HAS_JSON not defined)");
    (void)jsonPath; (void)textureBasePath;
    return false;
#else
    std::ifstream f(jsonPath);
    if (!f) { FG_WARN("TileMap: cannot open %s", jsonPath.c_str()); return false; }

    nlohmann::json j;
    try { f >> j; } catch (...) { FG_WARN("TileMap: JSON parse error in %s", jsonPath.c_str()); return false; }

    m_tileW = j.value("tilewidth",  32);
    m_tileH = j.value("tileheight", 32);
    m_mapW  = j.value("width",       0);
    m_mapH  = j.value("height",      0);

    m_tilesets.clear();
    for (auto& ts : j["tilesets"]) {
        Tileset t;
        t.firstGid = ts.value("firstgid",  1);
        t.tileW    = ts.value("tilewidth",  m_tileW);
        t.tileH    = ts.value("tileheight", m_tileH);
        t.cols     = ts.value("columns",    1);
        int total  = ts.value("tilecount",  t.cols);
        t.rows     = (t.cols > 0) ? (total / t.cols) : 1;

        std::string imgPath = textureBasePath + ts.value("image", std::string{});
        try {
            t.ownedTex = std::make_unique<Texture>(imgPath);
            t.texture  = t.ownedTex.get();
        } catch (...) {
            FG_WARN("TileMap: failed to load tileset image '%s'", imgPath.c_str());
        }
        m_tilesets.push_back(std::move(t));
    }

    m_layers.clear();
    for (auto& l : j["layers"]) {
        if (l.value("type", "") != "tilelayer") continue;
        Layer layer;
        layer.name = l.value("name", "Layer");
        auto& data = l["data"];
        layer.tiles.assign(data.begin(), data.end());
        m_layers.push_back(std::move(layer));
    }

    FG_INFO("TileMap loaded: %s  (%dx%d tiles, %zu layers)",
            jsonPath.c_str(), m_mapW, m_mapH, m_layers.size());
    return true;
#endif
}

// ---- Render ----------------------------------------------------------------

const TileMap::Tileset* TileMap::tilesetForGid(int gid) const {
    const Tileset* result = nullptr;
    for (const auto& ts : m_tilesets)
        if (ts.firstGid <= gid) result = &ts;
    return result;
}

void TileMap::render(const glm::vec2& origin) const {
    for (const auto& layer : m_layers) {
        for (int row = 0; row < m_mapH; ++row) {
            for (int col = 0; col < m_mapW; ++col) {
                int gid = layer.tiles[row * m_mapW + col];
                if (gid == 0) continue;

                const Tileset* ts = tilesetForGid(gid);
                if (!ts || !ts->texture) continue;

                // row 0 = top of map; Y-up world space means top = high Y
                float wx = origin.x + (float)(col       * m_tileW);
                float wy = origin.y + (float)((m_mapH - 1 - row) * m_tileH);

                Renderer2D::drawSubTexture({wx, wy},
                                          {(float)m_tileW, (float)m_tileH},
                                          *ts->texture, ts->getUV(gid));
            }
        }
    }
}

} // namespace forge
