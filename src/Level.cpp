#include "Level.hpp"
#include "Globals.hpp"

#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <map>
#include <utility>

Level::Level() {
    this->reset();
}

void Level::set_tile(Level::Layer layer, int row, int col, const Tile_Data &data) {

    if (data.texture != nullptr) {
        int cur_rows = this->layers[(int)Layer::GROUND].size();
        int cur_cols = this->layers[(int)Layer::GROUND][0].size();

        if (row < 0) {
            for (int rows_to_prepend = -row; rows_to_prepend > 0; rows_to_prepend -= 1) {
                for (int l = 0; l < LAYER_COUNT; l += 1) {
                    this->layers[l].emplace(this->layers[l].begin(), cur_cols);
                }
                row += 1;
            }
        } else {
            for (int rows_to_append = row + 1 - cur_rows; rows_to_append > 0; rows_to_append -= 1) {
                for (int l = 0; l < LAYER_COUNT; l += 1) {
                    this->layers[l].emplace_back(cur_cols);
                }
            }
        }

        if (col < 0) {
            for (int cols_to_prepend = -col; cols_to_prepend > 0; cols_to_prepend -= 1) {
                for (int l = 0; l < LAYER_COUNT; l += 1) {
                    for (auto &cols : this->layers[l]) {
                        cols.emplace(cols.begin());
                    }
                }
                col += 1;
            }
        } else {
            for (int cols_to_append = col + 1 - cur_cols; cols_to_append > 0; cols_to_append -= 1) {
                for (int l = 0; l < LAYER_COUNT; l += 1) {
                    for (auto &cols : this->layers[l]) {
                        cols.emplace_back();
                    }
                }
            }
        }
    }

    if (row >= 0
    &&  row < (int)this->layers[(int)layer].size()
    &&  col >= 0
    &&  col < (int)this->layers[(int)layer].size()) {

        this->layers[(int)layer][row][col] = data;
    }

    this->dirty = true;
}

void Level::render_layer(Level::Layer layer, int view_x_off, int view_y_off, float zoom) {
    if (this->dirty) {
        for (int l = 0; l < LAYER_COUNT; l += 1) {
            if (this->textures[l] != nullptr) {
                SDL_DestroyTexture(this->textures[l]);
                this->textures[l] = nullptr;
            }

            int rows = this->layers[l].size();
            int cols = rows > 0 ? this->layers[l][0].size() : 0;

            this->textures[l] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                                                  cols * TILE_SIZE, rows * TILE_SIZE);

            SDL_SetTextureScaleMode(this->textures[l], SDL_SCALEMODE_NEAREST);

            auto save_render_target = SDL_GetRenderTarget(renderer);

            SDL_SetRenderTarget(renderer, this->textures[l]);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);

            for (int row = 0; row < rows; row += 1) {
                for (int col = 0; col < cols; col += 1) {
                    Tile_Data &data = this->layers[l][row][col];
                    if (data.texture != nullptr) {
                        SDL_FRect src = { (float)TILE_SIZE * data.x, (float)TILE_SIZE * data.y, (float)TILE_SIZE, (float)TILE_SIZE };
                        SDL_FRect dst = { (float)TILE_SIZE * col, (float)TILE_SIZE * row, (float)TILE_SIZE, (float)TILE_SIZE };
                        SDL_RenderTexture(renderer, data.texture, &src, &dst);
                    }
                }
            }

            SDL_SetRenderTarget(renderer, save_render_target);
        }

        this->dirty = false;
    }

    SDL_FRect dst = { (float)view_x_off, (float)view_y_off, 0.0, 0.0 };

    SDL_GetTextureSize(this->textures[(int)layer], &dst.w, &dst.h);
    dst.w *= zoom;
    dst.h *= zoom;
    SDL_RenderTexture(renderer, this->textures[(int)layer], NULL, &dst);
}

int Level::get_height() { return this->layers[(int)Layer::GROUND].size();    }
int Level::get_width()  { return this->layers[(int)Layer::GROUND][0].size(); }

void Level::reset() {
    for (int l = 0; l < LAYER_COUNT; l += 1) {
        if (this->textures[l] != nullptr) {
            SDL_DestroyTexture(this->textures[l]);
            this->textures[l] = nullptr;
        }
        this->layers[l].clear();
        this->layers[l].resize(1);
        this->layers[l][0].resize(1);
    }
    this->dirty = 1;
}

void Level::save(std::string path) {
    std::filesystem::path file_path = path;

    std::filesystem::create_directories(file_path.parent_path());

    std::ofstream f(path);

    f << "dims" << "\t" << this->get_height() << "\t" << this->get_width() << "\n";

    std::map<SDL_Texture*, int> texture_ids;
    int texture_id = 0;
    for (auto &texture : texture_manager) {
        texture_ids[texture.second] = texture_id;

        f << "texture" << "\t" << texture_id << "\t" << texture.first << "\n";

        texture_id += 1;
    }

    for (int l = 0; l < LAYER_COUNT; l += 1) {
        f << "layer" << "\t" << l << "\n";
        for (int row = 0; row < this->get_height(); row += 1) {
            for (int col = 0; col < this->get_width(); col += 1) {
                const Tile_Data &data = this->layers[l][row][col];
                if (data.texture == nullptr) { continue; }

                f << "tile" << "\t"
                    << row << "\t"
                    << col << "\t"
                    << texture_ids[data.texture] << "\t"
                    << data.x << "\t"
                    << data.y << "\t"
                    << "\n";
            }
        }
    }

    f.close();
}

void Level::load(std::string path) {
    this->reset();

    std::filesystem::path file_path = path;

    std::ifstream f(path);

    std::string                line;
    std::string                tok;
    std::stringstream          line_ss;
    std::string                cmd;
    std::map<int, std::string> texture_id_to_name;
    std::string                texture_name;
    int                        cur_layer = (int)Layer::GROUND;

    auto next_token = [&]() -> std::stringstream {
        std::getline(line_ss, tok, '\t');
        return std::stringstream(tok);
    };


    while (std::getline(f, line)) {
        line_ss = std::stringstream(line);

        next_token() >> cmd;

        if (cmd == "dims") {
            int h;
            int w;

            next_token() >> h;
            next_token() >> w;

            for (int l = 0; l < LAYER_COUNT; l += 1) {
                this->layers[l].resize(h, {});
                for (auto &cols : this->layers[l]) {
                    cols.resize(w, {});
                }
            }
        } else if (cmd == "texture") {
            int id;
            next_token() >> id;
            next_token() >> texture_name;
            texture_id_to_name[id] = std::move(texture_name);
        } else if (cmd == "layer") {
            next_token() >> cur_layer;
        } else if (cmd == "tile") {
            int row;
            int col;
            int texture_id;
            int x;
            int y;

            next_token() >> row;
            next_token() >> col;
            next_token() >> texture_id;
            next_token() >> x;
            next_token() >> y;

            Tile_Data &data = this->layers[cur_layer][row][col];

            data.texture = texture_manager.get_texture(texture_id_to_name[texture_id]);
            data.x       = x;
            data.y       = y;
        }
    }

    f.close();

    this->dirty = true;
}
