#pragma once

#include <vector>
#include <string>

#include <SDL3/SDL.h>

struct Tile_Data {
    SDL_Texture *texture;
    int          x;
    int          y;
};

struct Level {
    enum class Layer {
        GROUND = 0,
        GROUND_COVER,
        SOLID,
        TOP,

        _COUNT,
    };

    static constexpr int LAYER_COUNT = (int)Layer::_COUNT;
    static constexpr int TILE_SIZE   = 16;

    std::vector<std::vector<Tile_Data>>  layers[LAYER_COUNT];
    SDL_Texture                         *textures[LAYER_COUNT] = {};
    bool                                 dirty = true;

    Level();
    void set_tile(Level::Layer layer, int row, int col, const Tile_Data &data);
    void render_layer(Level::Layer layer, int view_x_off, int view_y_off, float zoom);
    int get_height();
    int get_width();
    void reset();
    void save(std::string path);
    void load(std::string path);
};
