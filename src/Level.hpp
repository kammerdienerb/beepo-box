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
    enum {
        LAYER_GROUND = 0,
        LAYER_GROUND_COVER1,
        LAYER_GROUND_COVER2,
        LAYER_GROUND_COVER3,
        LAYER_SOLID,
        LAYER_TOP1,
        LAYER_TOP2,
        LAYER_TOP3,

        LAYER_COUNT,
    };

    static constexpr int TILE_SIZE = 16;

    std::vector<std::vector<Tile_Data>>  layers[LAYER_COUNT];
    SDL_Texture                         *textures[LAYER_COUNT] = {};
    bool                                 dirty = true;

    Level();
    void set_tile(int layer, int row, int col, const Tile_Data &data);
    void render_layer(int layer, int view_x_off, int view_y_off, float zoom);
    int get_height();
    int get_width();
    void reset();
    void save(std::string path);
    void load(std::string path);
};
