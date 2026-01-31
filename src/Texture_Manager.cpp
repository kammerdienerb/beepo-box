#include "Texture_Manager.hpp"
#include "Globals.hpp"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <filesystem>
#include <cstdio>

Texture_Manager::Texture_Manager() { }

static SDL_Texture * load_texture(const std::string &path) {
    SDL_Texture * texture = IMG_LoadTexture(renderer, path.c_str());

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    return texture;
}

Texture_Manager::Texture_Manager(const char *assets_path) {
    std::filesystem::path dir_path(assets_path);

    if (!std::filesystem::exists(dir_path) || !std::filesystem::is_directory(dir_path)) { return; }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir_path)) {
        if (entry.is_regular_file()) {
            if (entry.path().extension() == ".png") {
                std::string name = entry.path().string();
                this->add_texture(name, load_texture(name));
            }
        }
    }
}

void Texture_Manager::add_texture(const std::string &name, SDL_Texture *texture) {
    SDL_Texture *existing = this->textures[name];

    if (existing != nullptr) {
        SDL_DestroyTexture(existing);
    }

    this->textures[name] = texture;
}

SDL_Texture *Texture_Manager::get_texture(std::string name) {
    auto it = this->textures.find(name);
    return (it == this->textures.end()) ? nullptr : it->second;
}

decltype(Texture_Manager::textures)::iterator Texture_Manager::begin() { return this->textures.begin(); }
decltype(Texture_Manager::textures)::iterator Texture_Manager::end()   { return this->textures.end();   }
